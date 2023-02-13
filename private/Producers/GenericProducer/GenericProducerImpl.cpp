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

void DumpSceneDatabase(const cd::SceneDatabase& sceneDatabase)
{
	printf("Node count : %d\n", sceneDatabase.GetNodeCount());
	printf("Bone count : %d\n", sceneDatabase.GetBoneCount());
	printf("Mesh count : %d\n", sceneDatabase.GetMeshCount());
	printf("Material count : %d\n", sceneDatabase.GetMaterialCount());
	printf("Texture count : %d\n", sceneDatabase.GetTextureCount());
	printf("\n");

	for (const auto& node : sceneDatabase.GetNodes())
	{
		printf("[Node %u] ParentID : %u, Name : %s\n", node.GetID().Data(), node.GetParentID().Data(), node.GetName().c_str());

		for (cd::MeshID meshID : node.GetMeshIDs())
		{
			const auto& mesh = sceneDatabase.GetMesh(meshID.Data());
			printf("\t[MeshID %u] name : %s, face number : %u, vertex number : %u, materialID : %u\n", meshID.Data(), mesh.GetName(),
				mesh.GetPolygonCount(), mesh.GetVertexCount(), mesh.GetMaterialID().Data());
		}
	}

	for (const auto& material : sceneDatabase.GetMaterials())
	{
		printf("[MaterialID %u] %s\n", material.GetID().Data(), material.GetName());

		for (const auto& [materialTextureType, textureID] : material.GetTextureIDMap())
		{
			const auto& texture = sceneDatabase.GetTexture(textureID.Data());
			printf("\t[TextureID %u] %s - %s\n", textureID.Data(),
				GetMaterialTextureTypeName(materialTextureType), texture.GetPath());
		}
	}

	for (auto& bone : sceneDatabase.GetBones())
	{
		printf("[Bone %u] ParentID : %u, Name : %s\n", bone.GetID().Data(), bone.GetParentID().Data(), bone.GetName().c_str());
	}
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
		materialTextureMapping[aiTextureType_METALNESS] = cd::MaterialTextureType::Metalness;
		materialTextureMapping[aiTextureType_DIFFUSE_ROUGHNESS] = cd::MaterialTextureType::Roughness;
		materialTextureMapping[aiTextureType_AMBIENT_OCCLUSION] = cd::MaterialTextureType::AO;
		materialTextureMapping[aiTextureType_LIGHTMAP] = cd::MaterialTextureType::AO;
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
	cd::Material material(materialID, finalMaterialName.c_str());

	// Process all textures
	for (const auto& [textureType, materialTextureType] : materialTextureMapping)
	{
		// Multiple assimp texture types will map to the same texture to increase the successful rate.
		// Setup means one assimp texture type already added. Just skip remain assimp texture types.
		if (material.IsTextureTypeSetup(materialTextureType))
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

			material.SetTextureID(materialTextureType, textureID);

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

	uint32_t boneCount = pSourceMesh->mNumBones;
	for (uint32_t boneIndex = 0U; boneIndex < boneCount; ++boneIndex)
	{
		const aiBone* pSourceBone = pSourceMesh->mBones[boneIndex];
		bool isBoneReused = false;
		std::string boneName(pSourceBone->mName.C_Str());
		cd::BoneID::ValueType boneHash = cd::StringHash<cd::BoneID::ValueType>(boneName);
		cd::BoneID boneID = m_boneIDGenerator.AllocateID(boneHash, &isBoneReused);
		if (!isBoneReused)
		{
			//printf("\t[InitBone %u] Name : %s\n", boneID.Data(), boneName.c_str());
			// Other hierarchy data will be added in the post-process stage.
			cd::Bone bone(boneID, cd::MoveTemp(boneName));
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

void GenericProducerImpl::AddNode(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID)
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

	//printf("\t[InitNode %u] ParentID : %u, Name : %s\n", nodeID, sceneNode.GetParentID().Data(), sceneNode.GetName().c_str());

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
		AddNode(pSceneDatabase, pSourceScene, pSourceNode->mChildren[childIndex], childNodeID);
	}
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

void GenericProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	std::filesystem::path fileFolderPath = m_filePath;
	m_folderPath = fileFolderPath.parent_path().generic_string();

	printf("ImportSceneFile : %s\n", m_filePath.c_str());

	aiPropertyStore* pImportProperties = aiCreatePropertyStore();
	assert(pImportProperties);

	// Assimp will generate extra nodes as a chain for bone hierarchy if every bone includes data except basic Translation/Rotation/Scale.
	// In the first version, we want to make animation not so complex.
	// If you meet the case that needs more data, report this issue to us. Thanks!
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

	pSceneDatabase->SetName(m_filePath.c_str());

	// Process all nodes.
	aiNode* pSceneRootNode = pScene->mRootNode;
	pSceneDatabase->SetNodeCount(GetSceneNodesCount(pSceneRootNode));
	pSceneDatabase->SetMeshCount(pScene->mNumMeshes);
	AddNode(pSceneDatabase, pScene, pSceneRootNode, m_nodeIDGenerator.AllocateID());

	// Post-process meshes.
	cd::AABB sceneAABB(0.0f, 0.0f);
	std::optional<std::set<uint32_t>> optUsedMaterialIndexes = std::nullopt;
	if (IsCleanUnusedServiceActive())
	{
		optUsedMaterialIndexes = std::set<uint32_t>();
	}
	
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		// Merge a total AABB for all meshes in the scene.
		sceneAABB.Merge(mesh.GetAABB());

		// Query mesh used material indexes.
		if (optUsedMaterialIndexes.has_value())
		{
			optUsedMaterialIndexes.value().insert(mesh.GetMaterialID().Data());
		}
	}
	pSceneDatabase->SetAABB(cd::MoveTemp(sceneAABB));

	// Post-process bones.
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

	std::sort(removeNodeIndexes.begin(), removeNodeIndexes.end(), [](uint32_t lhs, uint32_t rhs) { return lhs > rhs; });
	for (uint32_t nodeIndex : removeNodeIndexes)
	{
		auto& sceneNodes = pSceneDatabase->GetNodes();
		sceneNodes.erase(sceneNodes.begin() + nodeIndex);
	}

	// Process all materials and used textures.
	uint32_t actualMaterialCount = optUsedMaterialIndexes.has_value() ? static_cast<uint32_t>(optUsedMaterialIndexes.value().size()) : pScene->mNumMaterials;
	pSceneDatabase->SetMaterialCount(actualMaterialCount);
	
	for (uint32_t materialIndex = 0; materialIndex < pScene->mNumMaterials; ++materialIndex)
	{
		// Skip parsing unused materials.
		if (optUsedMaterialIndexes.has_value() && !optUsedMaterialIndexes.value().contains(materialIndex))
		{
			continue;
		}

		AddMaterial(pSceneDatabase, pScene->mMaterials[materialIndex]);
	}

	DumpSceneDatabase(*pSceneDatabase);

	aiReleaseImport(pScene);
	pScene = nullptr;
}

}