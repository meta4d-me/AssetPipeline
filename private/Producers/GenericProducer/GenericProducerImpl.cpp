#include "GenericProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Scene/ObjectIDGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/Utils.h"

//#define ASSIMP_BUILD_NO_ARMATUREPOPULATE_PROCESS
#include <assimp/cimport.h>
#include <assimp/GltfMaterial.h>
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

// Assimp matrix is row major which needs a transpose to convert to cd::Matrix4x4.
cd::Matrix4x4 ConvertAssimpMatrix(const aiMatrix4x4& matrix)
{
	return cd::Matrix4x4(
		matrix.a1, matrix.b1, matrix.c1, matrix.d1,
		matrix.a2, matrix.b2, matrix.c2, matrix.d2,
		matrix.a3, matrix.b3, matrix.c3, matrix.d3,
		matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

cd::TextureMapMode ConvertAssimpTextureMapMode(aiTextureMapMode mapMode)
{
	switch (mapMode)
	{
	case aiTextureMapMode_Wrap:
		return cd::TextureMapMode::Wrap;
	case aiTextureMapMode_Clamp:
		return cd::TextureMapMode::Clamp;
	case aiTextureMapMode_Mirror:
		return cd::TextureMapMode::Mirror;
	case aiTextureMapMode_Decal:
	case _aiTextureMapMode_Force32Bit:
	default:
		assert("Need to support more texture map mode.");
		return cd::TextureMapMode::Wrap;
	}
}

}

namespace cdtools
{

GenericProducerImpl::GenericProducerImpl(std::string filePath) :
	m_filePath(cd::MoveTemp(filePath))
{
	// Default import options.
	m_options.Enable(GenericProducerOptions::CleanUnusedObjects);
	m_options.Enable(GenericProducerOptions::TriangulateModel);
	m_options.Enable(GenericProducerOptions::GenerateBoundingBox);
	m_options.Enable(GenericProducerOptions::OptimizeMeshBufferCacheHitRate);
	m_options.Enable(GenericProducerOptions::GenerateTangentSpace);
}

uint32_t GenericProducerImpl::GetImportFlags() const
{
	constexpr uint32_t BasicImportModelFlags[] =
	{
		aiProcess_ConvertToLeftHanded,
	};

	// Speed first. Only open flags which are necessary.
	constexpr uint32_t DefaultImportModelFlags = cdtools::array_sum(BasicImportModelFlags);

	uint32_t importFlags = DefaultImportModelFlags;
	if (IsOptionEnabled(GenericProducerOptions::TriangulateModel))
	{
		importFlags |= aiProcess_Triangulate;
	}

	if (IsOptionEnabled(GenericProducerOptions::GenerateBoundingBox))
	{
		importFlags |= aiProcess_GenBoundingBoxes;
	}

	if (IsOptionEnabled(GenericProducerOptions::FlattenTransformHierarchy))
	{
		importFlags |= aiProcess_PreTransformVertices;
	}

	if (IsOptionEnabled(GenericProducerOptions::GenerateTangentSpace))
	{
		importFlags |= aiProcess_CalcTangentSpace;
	}

	if (IsOptionEnabled(GenericProducerOptions::OptimizeMeshBufferCacheHitRate))
	{
		importFlags |= aiProcess_ImproveCacheLocality;
	}

	return importFlags;
}

void GenericProducerImpl::AddScene(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene)
{
	// TODO : it is not ideal as we will import many scenes to the SceneDatabase.
	// Multiple SceneDatabase vs Multiple Scenes in one SceneDatabase.
	pSceneDatabase->SetName(m_filePath.c_str());

	if (pSourceScene->HasMeshes())
	{
		// Add nodes and associated meshes to SceneDatabase.
		// For assimp, bones are also treated as nodes.
		AddNodeRecursively(pSceneDatabase, pSourceScene, pSourceScene->mRootNode, m_nodeIDGenerator.AllocateID().Data());
	}

	// Prepare to add materials.
	if (pSourceScene->HasMaterials())
	{
		AddMaterials(pSceneDatabase, pSourceScene);
	}
}

void GenericProducerImpl::AddNodeRecursively(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID)
{
	cd::NodeID sceneNodeID(nodeID);
	cd::Node sceneNode(sceneNodeID, pSourceNode->mName.C_Str());

	cd::Matrix4x4 transformation = ConvertAssimpMatrix(pSourceNode->mTransformation);
	cd::Transform newTransform(transformation.GetTranslation(), cd::Quaternion::FromMatrix(transformation.GetRotation()), transformation.GetScale());
	sceneNode.SetTransform(cd::MoveTemp(newTransform));

	// Cache it for searching parent node id.
	m_aiNodeToNodeIDLookup[pSourceNode] = nodeID;
	if (pSourceNode->mParent)
	{
		// Parent node ID should be queried because we are doing Depth-First search.
		const auto itParentNodeID = m_aiNodeToNodeIDLookup.find(pSourceNode->mParent);
		assert(itParentNodeID != m_aiNodeToNodeIDLookup.end() && "Failed to query parent node ID in scene database.");
		sceneNode.SetParentID(cd::NodeID(itParentNodeID->second));
	}

	// Add meshes from node.
	for (uint32_t meshIndex = 0; meshIndex < pSourceNode->mNumMeshes; ++meshIndex)
	{
		uint32_t sceneMeshIndex = pSourceNode->mMeshes[meshIndex];
		cd::MeshID meshID = AddMesh(pSceneDatabase, pSourceScene->mMeshes[sceneMeshIndex]);
		sceneNode.AddMeshID(meshID);
	}

	std::vector<uint32_t> childNodeIDs;
	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		cd::NodeID childNodeID = m_nodeIDGenerator.AllocateID();
		sceneNode.AddChildID(cd::NodeID(childNodeID.Data()));
		childNodeIDs.push_back(childNodeID.Data());
	}

	pSceneDatabase->AddNode(cd::MoveTemp(sceneNode));

	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		uint32_t childNodeID = childNodeIDs[childIndex];
		AddNodeRecursively(pSceneDatabase, pSourceScene, pSourceNode->mChildren[childIndex], childNodeID);
	}
}

cd::MeshID GenericProducerImpl::AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh)
{
	assert(pSourceMesh->mFaces && pSourceMesh->mNumFaces > 0 && "No polygon data.");

	uint32_t numVertices = pSourceMesh->mNumVertices;
	assert(pSourceMesh->mVertices && numVertices > 0 && "No vertex data.");

	std::stringstream meshHashString;
	// TODO : this hash is good to reuse mesh instance, but break the rule id same to index. Wait to have a explicit mesh instance support.
	// meshHashString << pSourceMesh->mName.C_Str() << "_" << pSourceMesh->mVertices << "_" << pSourceMesh->mFaces;
	meshHashString << pSourceMesh->mName.C_Str() << "_" << pSceneDatabase->GetMeshCount();

	cd::MeshID::ValueType meshHash = cd::StringHash<cd::MeshID::ValueType>(meshHashString.str());
	cd::MeshID meshID = m_meshIDGenerator.AllocateID(meshHash);
	cd::Mesh mesh(meshID, pSourceMesh->mName.C_Str(), numVertices, pSourceMesh->mNumFaces);
	mesh.SetMaterialID(cd::MaterialID(m_materialIDGenerator.GetCurrentID() + pSourceMesh->mMaterialIndex));

	// By default, aabb will be empty.
	if (IsOptionEnabled(GenericProducerOptions::GenerateBoundingBox))
	{
		cd::AABB meshAABB(cd::Point(pSourceMesh->mAABB.mMin.x, pSourceMesh->mAABB.mMin.y, pSourceMesh->mAABB.mMin.z),
			cd::Point(pSourceMesh->mAABB.mMax.x, pSourceMesh->mAABB.mMax.y, pSourceMesh->mAABB.mMax.z));
		mesh.SetAABB(cd::MoveTemp(meshAABB));
	}

	for (uint32_t faceIndex = 0; faceIndex < pSourceMesh->mNumFaces; ++faceIndex)
	{
		const aiFace& face = pSourceMesh->mFaces[faceIndex];
		assert(face.mNumIndices == 3 && "Do you forget to open importer's triangulate flag?");

		mesh.SetPolygon(faceIndex, { face.mIndices[0], face.mIndices[1], face.mIndices[2] });
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

	mesh.SetVertexFormat(cd::MoveTemp(meshVertexFormat));
	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
	return meshID;
}

cd::MaterialID GenericProducerImpl::AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial)
{
	// Mapping assimp material key to pbr based material key.
	static std::unordered_map<aiTextureType, cd::MaterialTextureType> materialTextureMapping;
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
		finalMaterialName = "untitled_";
		finalMaterialName += std::to_string(pSceneDatabase->GetMaterialCount());
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

	// Create a base PBR material type by default.
	cd::Material material(materialID, finalMaterialName.c_str(), cd::MaterialType::BasePBR);

	// Process all parameters

	// Albedo
	aiColor4D aiColor;
	cd::Vec3f cdColor{ 0.2f, 0.2f , 0.2f };
	if (aiReturn_SUCCESS == aiGetMaterialColor(pSourceMaterial, AI_MATKEY_BASE_COLOR, &aiColor))
	{
		cdColor = cd::Vec3f{ aiColor.r, aiColor.g , aiColor.b };
	}
	material.SetVec3fProperty(cd::MaterialPropertyGroup::BaseColor, cd::MaterialProperty::Color, cdColor);

	// Merallic
	float metallic = 0.1f;
	aiGetMaterialFloat(pSourceMaterial, AI_MATKEY_METALLIC_FACTOR, &metallic);
	material.SetFloatProperty(cd::MaterialPropertyGroup::Metallic, cd::MaterialProperty::Factor, metallic);

	// Roughness
	float roughness = 0.9f;
	aiGetMaterialFloat(pSourceMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &roughness);
	material.SetFloatProperty(cd::MaterialPropertyGroup::Roughness, cd::MaterialProperty::Factor, roughness);

	int twoSided = 0;
	aiGetMaterialInteger(pSourceMaterial, AI_MATKEY_TWOSIDED, &twoSided);
	material.SetBoolProperty(cd::MaterialPropertyGroup::General, cd::MaterialProperty::TwoSided, twoSided == 1);

	// TODO : only valid for gltf, actually it is generic producer.
	cd::BlendMode blendMode = cd::BlendMode::Opaque;
	if (std::filesystem::path filePath = m_filePath; ".gltf" == filePath.extension())
	{
		aiString blendModeName;
		aiGetMaterialString(pSourceMaterial, AI_MATKEY_GLTF_ALPHAMODE, &blendModeName);
		if (0 == strcmp("OPAQUE", blendModeName.C_Str()))
		{
			blendMode = cd::BlendMode::Opaque;
		}
		else if (0 == strcmp("MASK", blendModeName.C_Str()))
		{
			blendMode = cd::BlendMode::Mask;

			float alphaCutOff = 1.0f;
			aiGetMaterialFloat(pSourceMaterial, AI_MATKEY_GLTF_ALPHACUTOFF, &alphaCutOff);
			material.SetFloatProperty(cd::MaterialPropertyGroup::General, cd::MaterialProperty::OpacityMaskClipValue, alphaCutOff);
		}
		else if (0 == strcmp("BLEND", blendModeName.C_Str()))
		{
			blendMode = cd::BlendMode::Blend;
			//int blendFunc;
			//aiGetMaterialInteger(pSourceMaterial, AI_MATKEY_BLEND_FUNC, &blendFunc);
		}
	}
	material.SetI32Property(cd::MaterialPropertyGroup::General, cd::MaterialProperty::BlendMode, static_cast<int>(blendMode));
	
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
			material.SetBoolProperty(materialTextureType, cd::MaterialProperty::UseTexture, false);
			continue;
		}

		assert(textureCount == 1U && "Need to support multiple textures per type?");
		for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
		{
			aiString textureFilePath;
			aiTextureMapping textureMapping;
			uint32_t uvIndex;
			aiTextureMapMode textureMapMode[2];
			float blendFactor;
			aiTextureOp blendOperation;
			aiReturn result = aiGetMaterialTexture(pSourceMaterial, textureType, textureIndex, &textureFilePath,
				&textureMapping, &uvIndex, &blendFactor, &blendOperation, textureMapMode);
			assert(aiTextureMapping_UV == textureMapping && "Not UVMaping?");
			assert(0U == uvIndex && "Need to support non-zero uvIndex mapping.");
			if (aiReturn_SUCCESS != result ||
				aiTextureType_NONE == textureType ||
				aiTextureType_UNKNOWN == textureType)
			{
				continue;
			}
			material.SetBoolProperty(materialTextureType, cd::MaterialProperty::UseTexture, true);

			bool isTextureReused;
			uint32_t textureHash = cd::StringHash<cd::TextureID::ValueType>(textureFilePath.C_Str());
			cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, &isTextureReused);
			material.SetTextureID(materialTextureType, textureID);

			// Parse tiling parameters.
			aiUVTransform uvTransform;
			unsigned int maxBytes = sizeof(aiUVTransform);
			aiGetMaterialFloatArray(pSourceMaterial, AI_MATKEY_UVTRANSFORM(textureType, textureIndex), (float*)&uvTransform, &maxBytes);
		
			material.SetVec2fProperty(materialTextureType, cd::MaterialProperty::UVScale, cd::Vec2f(uvTransform.mScaling.x, uvTransform.mScaling.y));
			material.SetVec2fProperty(materialTextureType, cd::MaterialProperty::UVOffset, cd::Vec2f(uvTransform.mTranslation.x, uvTransform.mTranslation.y));
		
			// Reused textures don't need to add to SceneDatabase again.
			if (!isTextureReused)
			{
				std::filesystem::path textureName = textureFilePath.C_Str();
				textureName = textureName.filename();
				cd::Texture materialTexture(textureID, textureName.string().c_str(), materialTextureType);

				std::filesystem::path textureAbsolutePath = m_folderPath;
				textureAbsolutePath.append(textureFilePath.C_Str());
				materialTexture.SetPath(textureAbsolutePath.string().c_str());
				ConvertAssimpTextureMapMode(textureMapMode[0]), ConvertAssimpTextureMapMode(textureMapMode[1]);
				pSceneDatabase->AddTexture(cd::MoveTemp(materialTexture));
			}
			else
			{
				// Check if there are same file path but different tiling settings.
				// const cd::Texture& texture = pSceneDatabase->GetTexture(textureID.Data());
				// assert(texture.GetUVOffset().x() == uvTransform.mTranslation.x);
				// assert(texture.GetUVOffset().y() == uvTransform.mTranslation.y);
				// assert(texture.GetUVScale().x() == uvTransform.mScaling.x);
				// assert(texture.GetUVScale().y() == uvTransform.mScaling.y);
			}
		}
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));
	return materialID;
}

void GenericProducerImpl::AddMaterials(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene)
{
	std::optional<std::set<uint32_t>> optUsedMaterialIndexes = std::nullopt;
	if (IsOptionEnabled(GenericProducerOptions::CleanUnusedObjects))
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

void GenericProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	std::filesystem::path fileFolderPath = m_filePath;
	m_folderPath = fileFolderPath.parent_path().generic_string();

	printf("ImportSceneFile : %s\n", m_filePath.c_str());
	const aiScene* pScene = aiImportFile(m_filePath.c_str(), GetImportFlags());
	if (!pScene || !pScene->HasMeshes())
	{
		printf(aiGetErrorString());
		return;
	}

	if (pScene->mNumTextures > 0)
	{
		printf("[Unsupported] parse embedded textures.\n");
	}

	AddScene(pSceneDatabase, pScene);

	// Collect garbages in the end.
	aiReleaseImport(pScene);
	pScene = nullptr;
}

}