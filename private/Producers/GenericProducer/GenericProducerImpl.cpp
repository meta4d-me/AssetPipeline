#include "GenericProducerImpl.h"
#include "Hashers/StringHash.hpp"
#include "Scene/ObjectIDGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/Utils.h"

//#define ASSIMP_BUILD_NO_ARMATUREPOPULATE_PROCESS
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cassert>
#include <filesystem>
#include <optional>
#include <set>
#include <unordered_map>

namespace
{

uint32_t GetSceneNodesCount(const aiNode* pSceneNode)
{
	if (0U == pSceneNode->mNumChildren)
	{
		return 1U;
	}

	uint32_t totalCount = 0U;
	uint32_t childCount = pSceneNode->mNumChildren;
	for (uint32_t childIndex = 0U; childIndex < childCount; ++childIndex)
	{
		const aiNode* pChildNode = pSceneNode->mChildren[childIndex];
		totalCount += GetSceneNodesCount(pChildNode);
	}

	return totalCount;
}

// Assimp matrix is row major which needs a transpose to convert to cd::Matrix4x4.
cd::Matrix4x4 ConvertAssimpMatrix(const aiMatrix4x4& matrix)
{
	return cd::Matrix4x4(
		matrix.a1, matrix.b1, matrix.c1, matrix.d1,
		matrix.a2, matrix.b2, matrix.c2, matrix.d2,
		matrix.a3, matrix.b3, matrix.c3, matrix.d3,
		matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

}

namespace cdtools
{

uint32_t GenericProducerImpl::GetImportFlags() const
{
	constexpr uint32_t BasicImportModelFlags[] =
	{
		aiProcess_ConvertToLeftHanded,
		aiProcess_ImproveCacheLocality,		// Improve GPU vertex data cache miss rate. That is, ACMR.
	};

	// Speed first. Only open flags which are necessary.
	constexpr uint32_t DefaultImportModelFlags = cdtools::array_sum(BasicImportModelFlags);

	uint32_t importFlags = DefaultImportModelFlags;
	if (IsTriangulateServiceActive())
	{
		importFlags += aiProcess_Triangulate;
	}

	if (IsBoundingBoxServiceActive())
	{
		importFlags += aiProcess_GenBoundingBoxes;
	}

	if (IsFlattenHierarchyServiceActive())
	{
		importFlags += aiProcess_PreTransformVertices;
	}

	if (IsTangentsSpaceServiceActive())
	{
		importFlags += aiProcess_CalcTangentSpace;
	}

	return importFlags;
}

void GenericProducerImpl::SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID)
{
	m_nodeIDGenerator.SetCurrentID(nodeID);
	m_meshIDGenerator.SetCurrentID(meshID);
	m_materialIDGenerator.SetCurrentID(materialID);
	m_textureIDGenerator.SetCurrentID(textureID);
	m_lightIDGenerator.SetCurrentID(lightID);

	m_nodeIDToNodeIndexLookup.clear();
	m_aiNodeToNodeIDLookup.clear();
}

cd::MaterialID GenericProducerImpl::AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial)
{
	static std::unordered_map<aiTextureType, cd::MaterialTextureType> materialTextureMapping;
	{
		// Mapping assimp material key to pbr based material key.
		materialTextureMapping[aiTextureType_DIFFUSE] = cd::MaterialTextureType::BaseColor;
		materialTextureMapping[aiTextureType_BASE_COLOR] = cd::MaterialTextureType::BaseColor;
		materialTextureMapping[aiTextureType_NORMALS] = cd::MaterialTextureType::Normal;
		materialTextureMapping[aiTextureType_NORMAL_CAMERA] = cd::MaterialTextureType::Normal;
		materialTextureMapping[aiTextureType_EMISSIVE] = cd::MaterialTextureType::Emissive;
		materialTextureMapping[aiTextureType_EMISSION_COLOR] = cd::MaterialTextureType::Emissive;
		materialTextureMapping[aiTextureType_METALNESS] = cd::MaterialTextureType::Metallic;
		materialTextureMapping[aiTextureType_DIFFUSE_ROUGHNESS] = cd::MaterialTextureType::Roughness;
		materialTextureMapping[aiTextureType_AMBIENT_OCCLUSION] = cd::MaterialTextureType::Occlusion;
		materialTextureMapping[aiTextureType_LIGHTMAP] = cd::MaterialTextureType::Occlusion;
	}

	aiString materialName;
	if (aiReturn_SUCCESS != aiGetMaterialString(pSourceMaterial, AI_MATKEY_NAME, &materialName))
	{
		assert("\tMaterial name is not found.\n");
	}

	std::string finalMaterialName;
	if (materialName.length > 0)
	{
		finalMaterialName = materialName.C_Str();
	}
	else
	{
		finalMaterialName = "untitled_" + pSceneDatabase->GetMaterialCount();
		printf("\tWarning : current material doesn't have name?\n");
	}

	bool isMaterialReused;
	cd::MaterialID::ValueType materialHash = cd::StringHash<cd::MaterialID::ValueType>(finalMaterialName);
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, &isMaterialReused);
	if (isMaterialReused)
	{
		// Skip to parse material which should already exist in SceneDatabase.
		// Note that two materials with same configs but have different names will be considered as different materia.
		// Because we only use material name as hash value. Don't want to compare all parameters.
		return materialID;
	}

	//printf("\t[MaterialID %u] %s\n", materialID.Data(), finalMaterialName.c_str());
	
	// Create a base PBR material type by default.
	cd::Material material(materialID, finalMaterialName.c_str(), cd::MaterialType::BasePBR);

	// Process all textures
	for (const auto& [textureType, materialTextureType] : materialTextureMapping)
	{
		// Multiple assimp texture types will map to the same texture to increase the successful rate.
		// Setup means one assimp texture type already added. Just skip remain assimp texture types.
		if (material.IsTextureSetup(materialTextureType))
		{
			continue;
		}

		uint32_t textureCount = aiGetMaterialTextureCount(pSourceMaterial, textureType);
		if (0 == textureCount)
		{
			continue;
		}

		for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
		{
			aiString ai_path;
			aiReturn result = aiGetMaterialTexture(pSourceMaterial, textureType, textureIndex, &ai_path);
			if (aiReturn_SUCCESS != result)
			{
				//printf("\t\tFailed to read material texture property, textureType is %u, texture index is %u", textureType, textureIndex);
				continue;
			}

			if (textureType == aiTextureType_NONE)
			{
				//printf("\t\tTextureType is none? Investigate the cause please.");
				continue;
			}

			if (textureType == aiTextureType_UNKNOWN)
			{
				//printf("\t\tTextureType is unknown. Should find ways to map it.");
				continue;
			}

			bool isTextureReused;
			uint32_t textureHash = cd::StringHash<cd::TextureID::ValueType>(ai_path.C_Str());
			cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, &isTextureReused);

			std::filesystem::path textureAbsolutePath = m_folderPath;
			textureAbsolutePath.append(ai_path.C_Str());

			material.AddTextureID(materialTextureType, textureID);

			// Reused textures don't need to add to SceneDatabase again.
			if (!isTextureReused)
			{
				pSceneDatabase->AddTexture(cd::Texture(textureID, materialTextureType, textureAbsolutePath.generic_string().c_str()));
			}
		}
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));
	return materialID;
}

cd::MeshID GenericProducerImpl::AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh)
{
	assert(pSourceMesh->mFaces && pSourceMesh->mNumFaces > 0 && "No polygon data.");

	uint32_t numVertices = pSourceMesh->mNumVertices;
	assert(pSourceMesh->mVertices && numVertices > 0 && "No vertex data.");

	cd::MeshID::ValueType meshHash = cd::StringHash<cd::MeshID::ValueType>(pSourceMesh->mName.C_Str());
	cd::MeshID meshID = m_meshIDGenerator.AllocateID(meshHash);
	cd::Mesh mesh(meshID, pSourceMesh->mName.C_Str(), numVertices, pSourceMesh->mNumFaces);
	mesh.SetMaterialID(m_materialIDGenerator.GetCurrentID() + pSourceMesh->mMaterialIndex);

	// By default, aabb will be empty.
	if (IsBoundingBoxServiceActive())
	{
		cd::AABB meshAABB(cd::Point(pSourceMesh->mAABB.mMin.x, pSourceMesh->mAABB.mMin.y, pSourceMesh->mAABB.mMin.z),
			cd::Point(pSourceMesh->mAABB.mMax.x, pSourceMesh->mAABB.mMax.y, pSourceMesh->mAABB.mMax.z));
		mesh.SetAABB(cd::MoveTemp(meshAABB));
	}

	for (uint32_t faceIndex = 0; faceIndex < pSourceMesh->mNumFaces; ++faceIndex)
	{
		const aiFace& face = pSourceMesh->mFaces[faceIndex];
		assert(face.mNumIndices == 3 && "tjj : Do you forget to open importer's triangulate flag?");

		uint32_t originIndex0 = face.mIndices[0];
		uint32_t originIndex1 = face.mIndices[1];
		uint32_t originIndex2 = face.mIndices[2];

		uint32_t index0 = originIndex0;
		uint32_t index1 = originIndex1;
		uint32_t index2 = originIndex2;
		mesh.SetPolygon(faceIndex, cd::VertexID(index0), cd::VertexID(index1), cd::VertexID(index2));
	}

	cd::VertexFormat meshVertexFormat;
	assert(pSourceMesh->HasPositions() && "Mesh doesn't have vertex positions.");
	for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
	{
		uint32_t vertexDataIndex = vertexIndex;
		const aiVector3D& position = pSourceMesh->mVertices[vertexDataIndex];
		mesh.SetVertexPosition(vertexIndex, cd::Point(position.x, position.y, position.z));
	}
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);

	if (pSourceMesh->HasNormals())
	{
		for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
		{
			uint32_t vertexDataIndex = vertexIndex;
			const aiVector3D& normal = pSourceMesh->mNormals[vertexDataIndex];
			mesh.SetVertexNormal(vertexIndex, cd::Direction(normal.x, normal.y, normal.z));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);

		if (pSourceMesh->HasTangentsAndBitangents())
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				const aiVector3D& tangent = pSourceMesh->mTangents[vertexDataIndex];
				mesh.SetVertexTangent(vertexIndex, cd::Direction(tangent.x, tangent.y, tangent.z));
			}
			meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);


			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				const aiVector3D& biTangent = pSourceMesh->mBitangents[vertexDataIndex];
				mesh.SetVertexBiTangent(vertexIndex, cd::Direction(biTangent.x, biTangent.y, biTangent.z));
			}
			meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
		}
	}

	uint32_t uvSetCount = pSourceMesh->GetNumUVChannels();
	mesh.SetVertexUVSetCount(uvSetCount);

	for (uint32_t uvSetIndex = 0; uvSetIndex < uvSetCount; ++uvSetIndex)
	{
		const aiVector3D* vertexUVArray = pSourceMesh->mTextureCoords[uvSetIndex];
		uint32_t numUVComponents = pSourceMesh->mNumUVComponents[uvSetIndex];
		if (0 == numUVComponents)
		{
			// 2 means normal 2D texture
			// 3 means 3D texture such as cubemap
			continue;
		}

		for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
		{
			uint32_t vertexDataIndex = vertexIndex;
			const aiVector3D& uv = vertexUVArray[vertexDataIndex];
			mesh.SetVertexUV(uvSetIndex, vertexIndex, cd::UV(uv.x, uv.y));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}

	uint32_t colorSetCount = pSourceMesh->GetNumColorChannels();
	mesh.SetVertexColorSetCount(colorSetCount);

	for (uint32_t colorSetIndex = 0; colorSetIndex < colorSetCount; ++colorSetIndex)
	{
		const aiColor4D* vertexColorArray = pSourceMesh->mColors[colorSetIndex];
		for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
		{
			uint32_t vertexDataIndex = vertexIndex;
			const aiColor4D& color = vertexColorArray[vertexDataIndex];
			mesh.SetVertexColor(colorSetIndex, vertexIndex, cd::Color(color.r, color.g, color.b, color.a));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Color::ValueType>(), cd::Color::Size);
	}

	if(pSourceMesh->HasBones())
	{
		AddMeshBones(pSceneDatabase, pSourceMesh, mesh);

		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneIndex, cd::AttributeValueType::Int16, cd::MaxBoneInfluenceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneWeight, cd::AttributeValueType::Float, cd::MaxBoneInfluenceCount);
	}

	mesh.SetVertexFormat(cd::MoveTemp(meshVertexFormat));
	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
	return meshID;
}

void GenericProducerImpl::AddMeshBones(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh, cd::Mesh& mesh)
{
	std::map<uint32_t, uint32_t> mapVertexIndexToCurrentBoneCount;
	for (uint32_t boneIndex = 0U; boneIndex < pSourceMesh->mNumBones; ++boneIndex)
	{
		const aiBone* pSourceBone = pSourceMesh->mBones[boneIndex];
		bool isBoneReused = false;
		std::string boneName(pSourceBone->mName.C_Str());
		cd::BoneID::ValueType boneHash = cd::StringHash<cd::BoneID::ValueType>(boneName);
		cd::BoneID boneID = m_boneIDGenerator.AllocateID(boneHash, &isBoneReused);
		if (!isBoneReused)
		{
			cd::Bone bone(boneID, cd::MoveTemp(boneName));

			cd::Matrix4x4 transformation = ConvertAssimpMatrix(pSourceBone->mOffsetMatrix);
			bone.SetTransform(cd::Transform(transformation.GetTranslation(), cd::Quaternion::FromMatrix(transformation.GetRotation()), transformation.GetScale()));

			pSceneDatabase->AddBone(cd::MoveTemp(bone));
		}
		
		for (uint32_t influencedVertexIndex = 0U; influencedVertexIndex < pSourceBone->mNumWeights; ++influencedVertexIndex)
		{
			const aiVertexWeight& boneVertexWeight = pSourceBone->mWeights[influencedVertexIndex];
			uint32_t vertexIndex = boneVertexWeight.mVertexId;

			uint32_t currentBoneCount = 0U;
			auto itVertexBoneCount = mapVertexIndexToCurrentBoneCount.find(vertexIndex);
			if (itVertexBoneCount != mapVertexIndexToCurrentBoneCount.end())
			{
				currentBoneCount = mapVertexIndexToCurrentBoneCount[vertexIndex] + 1;
				assert(currentBoneCount <= cd::MaxBoneInfluenceCount);
			}

			mesh.SetVertexBoneWeight(currentBoneCount, vertexIndex, boneID, boneVertexWeight.mWeight);
			mapVertexIndexToCurrentBoneCount[vertexIndex] = currentBoneCount;
		}
	}
}

void GenericProducerImpl::AddAnimation(cd::SceneDatabase* pSceneDatabase, const aiAnimation* pSourceAnimation)
{
	const char* pAnimationName = pSourceAnimation->mName.C_Str();
	cd::AnimationID::ValueType animationHash = cd::StringHash<cd::AnimationID::ValueType>(pAnimationName);
	cd::AnimationID animationID = m_animationIDGenerator.AllocateID(animationHash);

	cd::Animation animation(animationID, pAnimationName);
	animation.SetDuration(static_cast<float>(pSourceAnimation->mDuration));

	for (uint32_t channelIndex = 0U; channelIndex < pSourceAnimation->mNumChannels; ++channelIndex)
	{
		const aiNodeAnim* pBoneTrack = pSourceAnimation->mChannels[channelIndex];

		const char* pTrackName = pBoneTrack->mNodeName.C_Str();
		cd::TrackID::ValueType trackHash = cd::StringHash<cd::TrackID::ValueType>(pTrackName);
		cd::TrackID trackID = m_trackIDGenerator.AllocateID(trackHash);

		cd::Track boneTrack(trackID, pTrackName);
		boneTrack.SetTranslationKeyCount(pBoneTrack->mNumPositionKeys);
		boneTrack.SetRotationKeyCount(pBoneTrack->mNumRotationKeys);
		boneTrack.SetScaleKeyCount(pBoneTrack->mNumScalingKeys);

		for (uint32_t translationKeyIndex = 0U; translationKeyIndex < boneTrack.GetTranslationKeyCount(); ++translationKeyIndex)
		{
			const aiVectorKey& sourcePositionKey = pBoneTrack->mPositionKeys[translationKeyIndex];
			const aiVector3D& rotationValue = sourcePositionKey.mValue;

			auto& translationKey = boneTrack.GetTranslationKeys()[translationKeyIndex];
			translationKey.SetTime(static_cast<float>(sourcePositionKey.mTime));
			translationKey.SetValue(cd::Vec3f(rotationValue.x, rotationValue.y, rotationValue.z));
		}

		for (uint32_t rotationKeyIndex = 0U; rotationKeyIndex < boneTrack.GetRotationKeyCount(); ++rotationKeyIndex)
		{
			const aiQuatKey& sourceRotationKey = pBoneTrack->mRotationKeys[rotationKeyIndex];
			const aiQuaternion& rotationValue = sourceRotationKey.mValue;

			auto& rotationKey = boneTrack.GetRotationKeys()[rotationKeyIndex];
			rotationKey.SetTime(static_cast<float>(sourceRotationKey.mTime));
			rotationKey.SetValue(cd::Quaternion(rotationValue.w, rotationValue.x, rotationValue.y, rotationValue.z));
		}

		for (uint32_t scaleKeyIndex = 0U; scaleKeyIndex < boneTrack.GetScaleKeyCount(); ++scaleKeyIndex)
		{
			const aiVectorKey& sourceScaleKey = pBoneTrack->mScalingKeys[scaleKeyIndex];
			const aiVector3D& scaleValue = sourceScaleKey.mValue;

			auto& scaleKey = boneTrack.GetScaleKeys()[scaleKeyIndex];
			scaleKey.SetTime(static_cast<float>(sourceScaleKey.mTime));
			scaleKey.SetValue(cd::Vec3f(scaleValue.x, scaleValue.y, scaleValue.z));
		}

		animation.AddBoneTrackID(trackID.Data());
		pSceneDatabase->AddTrack(cd::MoveTemp(boneTrack));
	}

	pSceneDatabase->AddAnimation(cd::MoveTemp(animation));
}

void GenericProducerImpl::AddNodeRecursively(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID)
{
	cd::NodeID sceneNodeID(nodeID);
	cd::Node sceneNode(sceneNodeID, pSourceNode->mName.C_Str());

	cd::Matrix4x4 transformation = ConvertAssimpMatrix(pSourceNode->mTransformation);
	sceneNode.SetTransform(cd::Transform(transformation.GetTranslation(), cd::Quaternion::FromMatrix(transformation.GetRotation()), transformation.GetScale()));

	// Cache it for searching parent node id.
	m_aiNodeToNodeIDLookup[pSourceNode] = nodeID;
	if (pSourceNode->mParent)
	{
		// Parent node ID should be queried because we are doing Depth-First search.
		const auto itParentNodeID = m_aiNodeToNodeIDLookup.find(pSourceNode->mParent);
		assert(itParentNodeID != m_aiNodeToNodeIDLookup.end() && "Failed to query parent node ID in scene database.");
		sceneNode.SetParentID(itParentNodeID->second);
	}

	// Add meshes from node.
	for (uint32_t meshIndex = 0; meshIndex < pSourceNode->mNumMeshes; ++meshIndex)
	{
		uint32_t sceneMeshIndex = pSourceNode->mMeshes[meshIndex];
		cd::MeshID meshID = AddMesh(pSceneDatabase, pSourceScene->mMeshes[sceneMeshIndex]);
		sceneNode.AddMeshID(meshID.Data());
	}

	std::vector<uint32_t> childNodeIDs;
	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		uint32_t childNodeID = m_nodeIDGenerator.AllocateID();
		sceneNode.AddChildID(childNodeID);
		childNodeIDs.push_back(childNodeID);
	}

	m_nodeIDToNodeIndexLookup[sceneNodeID.Data()] = pSceneDatabase->GetNodeCount();
	pSceneDatabase->AddNode(cd::MoveTemp(sceneNode));

	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		uint32_t childNodeID = childNodeIDs[childIndex];
		AddNodeRecursively(pSceneDatabase, pSourceScene, pSourceNode->mChildren[childIndex], childNodeID);
	}
}

void GenericProducerImpl::AddMaterials(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene)
{
	std::optional<std::set<uint32_t>> optUsedMaterialIndexes = std::nullopt;
	if (IsCleanUnusedServiceActive())
	{
		optUsedMaterialIndexes = std::set<uint32_t>();
	}

	// As we parsed meshes at first, so we can analyze how many materials are actually used.
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		// Query mesh used material indexes.
		if (optUsedMaterialIndexes.has_value())
		{
			optUsedMaterialIndexes.value().insert(mesh.GetMaterialID().Data());
		}
	}

	// Add materials and associated textures(a simple filepath or raw pixel data) to SceneDatabase.
	uint32_t actualMaterialCount = optUsedMaterialIndexes.has_value() ? static_cast<uint32_t>(optUsedMaterialIndexes.value().size()) : pSourceScene->mNumMaterials;
	pSceneDatabase->SetMaterialCount(actualMaterialCount);
	for (uint32_t materialIndex = 0; materialIndex < pSourceScene->mNumMaterials; ++materialIndex)
	{
		if (optUsedMaterialIndexes.has_value() &&
			!optUsedMaterialIndexes.value().contains(materialIndex))
		{
			// Skip parsing unused materials.
			continue;
		}

		AddMaterial(pSceneDatabase, pSourceScene->mMaterials[materialIndex]);
	}
}

void GenericProducerImpl::AddScene(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene)
{
	// TODO : it is not ideal as we will import many scenes to the SceneDatabase.
	// Multiple SceneDatabase vs Multiple Scenes in one SceneDatabase.
	pSceneDatabase->SetName(m_filePath.c_str());

	if (pSourceScene->HasMeshes())
	{
		pSceneDatabase->SetNodeCount(GetSceneNodesCount(pSourceScene->mRootNode));
		pSceneDatabase->SetMeshCount(pSourceScene->mNumMeshes);

		// Add nodes and associated meshes(also bones for SkinMesh) to SceneDatabase.
		AddNodeRecursively(pSceneDatabase, pSourceScene, pSourceScene->mRootNode, m_nodeIDGenerator.AllocateID());
	}

	// Prepare to add materials.
	if (pSourceScene->HasMaterials())
	{
		AddMaterials(pSceneDatabase, pSourceScene);
	}

	// Add animations.
	if (pSourceScene->HasAnimations())
	{
		pSceneDatabase->SetAnimationCount(pSourceScene->mNumAnimations);
		for (uint32_t animationIndex = 0U; animationIndex < pSourceScene->mNumAnimations; ++animationIndex)
		{
			AddAnimation(pSceneDatabase, pSourceScene->mAnimations[animationIndex]);
		}
	}
}

void GenericProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	std::filesystem::path fileFolderPath = m_filePath;
	m_folderPath = fileFolderPath.parent_path().generic_string();

	printf("ImportSceneFile : %s\n", m_filePath.c_str());

	aiPropertyStore* pImportProperties = aiCreatePropertyStore();
	assert(pImportProperties);

	// Assimp will generate extra nodes as a chain for bone hierarchy if every bone includes data except basic Translation/Rotation/Scale.
	// In the first version, we want to make animation not so complex.
	aiSetImportPropertyInteger(pImportProperties, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
	const aiScene* pScene = aiImportFileExWithProperties(m_filePath.c_str(), GetImportFlags(), nullptr, pImportProperties);

	aiReleasePropertyStore(pImportProperties);
	pImportProperties = nullptr;

	if (!pScene || !pScene->HasMeshes())
	{
		printf(aiGetErrorString());
		return;
	}
	assert(pScene->mNumTextures == 0 && "[Unsupported] parse embedded textures.");

	// Add scene.
	AddScene(pSceneDatabase, pScene);

	// Post-process bones.
	RemoveBoneReferenceNodes(pSceneDatabase);
	KeepNodeIDAndIndexSame(pSceneDatabase);

	// Collect garbages in the end.
	aiReleaseImport(pScene);
	pScene = nullptr;
}

// It is a specific behavior from assimp implementation which will import bones as nodes because their data structure is similiar to reuse.
// But we don't want to mess up these different two objects.
void GenericProducerImpl::RemoveBoneReferenceNodes(cd::SceneDatabase* pSceneDatabase)
{
	std::vector<uint32_t> removeNodeIndexes;
	for (auto& bone : pSceneDatabase->GetBones())
	{
		// Find node which has the same name with bone.
		// In assimp implementation, it reuses Node as Bone hierarchy which is not expected to us.
		const cd::Node* pBoneNode = pSceneDatabase->GetNodeByName(bone.GetName());
		if (!pBoneNode)
		{
			continue;
		}

		if (pBoneNode->GetParentID().IsValid())
		{
			const cd::Node& parentNode = pSceneDatabase->GetNode(m_nodeIDToNodeIndexLookup[pBoneNode->GetParentID().Data()]);
			if (const cd::Bone* pParentBone = pSceneDatabase->GetBoneByName(parentNode.GetName()))
			{
				bone.SetParentID(pParentBone->GetID().Data());
			}
		}

		for (const cd::NodeID& childNodeID : pBoneNode->GetChildIDs())
		{
			const cd::Node& childNode = pSceneDatabase->GetNode(m_nodeIDToNodeIndexLookup[childNodeID.Data()]);
			if (const cd::Bone* pChildBone = pSceneDatabase->GetBoneByName(childNode.GetName()))
			{
				bone.AddChildID(pChildBone->GetID().Data());
			}
		}

		removeNodeIndexes.push_back(m_nodeIDToNodeIndexLookup[pBoneNode->GetID().Data()]);
	}

	// Remove bone nodes
	auto& sceneNodes = pSceneDatabase->GetNodes();
	if (!removeNodeIndexes.empty())
	{
		std::sort(removeNodeIndexes.begin(), removeNodeIndexes.end(), [](uint32_t lhs, uint32_t rhs) { return lhs > rhs; });
		for (uint32_t nodeIndex : removeNodeIndexes)
		{
			sceneNodes.erase(sceneNodes.begin() + nodeIndex);
		}
	}
}

// By adding/removing bone as nodes, the node indexes and ids will be not same.
// This stage helps to loop the whole node hierarchy to reassign ids.
void GenericProducerImpl::KeepNodeIDAndIndexSame(cd::SceneDatabase* pSceneDatabase)
{
	auto& sceneNodes = pSceneDatabase->GetNodes();
	// Reorder ids
	assert(pSceneDatabase->GetNodeCount() == sceneNodes.size());
	std::map<uint32_t, uint32_t> oldToNewNodeID;
	uint32_t nodeIndex = 0U;
	for (auto& sceneNode : sceneNodes)
	{
		if (nodeIndex != sceneNode.GetID().Data())
		{
			auto itNode = oldToNewNodeID.find(sceneNode.GetID().Data());
			assert(itNode == oldToNewNodeID.end());
			oldToNewNodeID[sceneNode.GetID().Data()] = nodeIndex;
			sceneNode.SetID(cd::NodeID(nodeIndex));
		}

		++nodeIndex;
	}

	// Update every node's parent id and child ids. If there are other places which also store NodeID, it also should update.
	if (!oldToNewNodeID.empty())
	{
		for (auto& sceneNode : sceneNodes)
		{
			uint32_t parentID = sceneNode.GetParentID().Data();
			auto itModifiedIDNode = oldToNewNodeID.find(parentID);
			if (itModifiedIDNode != oldToNewNodeID.end())
			{
				sceneNode.SetParentID(itModifiedIDNode->second);
			}

			auto& childIDs = sceneNode.GetChildIDs();
			for (uint32_t childIndex = 0U, childCount = sceneNode.GetChildCount(); childIndex < childCount; ++childIndex)
			{
				uint32_t childNodeID = childIDs[childIndex].Data();
				auto itModifiedIDNode = oldToNewNodeID.find(childNodeID);
				if (itModifiedIDNode != oldToNewNodeID.end())
				{
					childIDs[childIndex] = cd::NodeID(itModifiedIDNode->second);
				}
			}
		}
	}
}

}