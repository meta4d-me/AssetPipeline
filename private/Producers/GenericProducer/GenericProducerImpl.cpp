#include "GenericProducerImpl.h"
#include "Hashers/StringHash.hpp"
#include "Scene/ObjectIDGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/Utils.h"

#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cassert>
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
		static int unnamedMaterialCount = 0;
		finalMaterialName = "untitled_";
		finalMaterialName += unnamedMaterialCount++;
		printf("\tWarning : current material doesn't have name?\n");
	}

	bool isMaterialReused;
	cd::MaterialID::ValueType materialHash = cd::StringHash<cd::MaterialID::ValueType>(finalMaterialName);
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, isMaterialReused);
	if (isMaterialReused)
	{
		// Skip to parse material which should already exist in SceneDatabase.
		// Note that two materials with same configs but have different names will be considered as different materia.
		// Because we only use material name as hash value. Don't want to compare all parameters.
		return materialID;
	}

	printf("\t[MaterialID %u] %s\n", materialID.Data(), finalMaterialName.c_str());
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
				printf("\t\tFailed to read material texture property, textureType is %u, texture index is %u", textureType, textureIndex);
				continue;
			}

			if (textureType == aiTextureType_NONE)
			{
				printf("\t\tTextureType is none? Investigate the cause please.");
				continue;
			}

			if (textureType == aiTextureType_UNKNOWN)
			{
				printf("\t\tTextureType is unknown. Should find ways to map it.");
				continue;
			}

			bool isTextureReused;
			uint32_t textureHash = cd::StringHash<cd::TextureID::ValueType>(ai_path.C_Str());
			cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, isTextureReused);
			printf("\t\t[TextureID %u] %s - %s\n",
				textureID.Data(),
				GetMaterialTextureTypeName(materialTextureType),
				ai_path.C_Str());

			material.SetTextureID(materialTextureType, textureID);

			// Reused textures don't need to add to SceneDatabase again.
			if (!isTextureReused)
			{
				pSceneDatabase->AddTexture(cd::Texture(textureID, ai_path.C_Str()));
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
	if (IsDuplicateVertexServiceActive())
	{
		numVertices = pSourceMesh->mNumFaces * 3;
	}

	assert(pSourceMesh->mVertices && numVertices > 0 && "No vertex data.");

	bool isMeshReused;
	cd::MeshID::ValueType meshHash = cd::StringHash<cd::MeshID::ValueType>(pSourceMesh->mName.C_Str());
	cd::MeshID meshID = m_meshIDGenerator.AllocateID(meshHash, isMeshReused);
	printf("\t[MeshID %u] face number : %u, vertex number : %u\n", meshID.Data(), pSourceMesh->mNumFaces, numVertices);

	cd::Mesh mesh(meshID, pSourceMesh->mName.C_Str(), numVertices, pSourceMesh->mNumFaces);
	mesh.SetMaterialID(pSourceMesh->mMaterialIndex);

	// By default, aabb will be empty.
	if (IsBoundingBoxServiceActive())
	{
		cd::AABB meshAABB(cd::Point(pSourceMesh->mAABB.mMin.x, pSourceMesh->mAABB.mMin.y, pSourceMesh->mAABB.mMin.z),
			cd::Point(pSourceMesh->mAABB.mMax.x, pSourceMesh->mAABB.mMax.y, pSourceMesh->mAABB.mMax.z));
		mesh.SetAABB(cd::MoveTemp(meshAABB));
	}

	std::map<uint32_t, uint32_t> mapNewIndexToOriginIndex;
	uint32_t currentVertexID = 0U;
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
		if (IsDuplicateVertexServiceActive())
		{
			index0 = currentVertexID;
			index1 = currentVertexID + 1;
			index2 = currentVertexID + 2;

			mapNewIndexToOriginIndex[index0] = originIndex0;
			mapNewIndexToOriginIndex[index1] = originIndex1;
			mapNewIndexToOriginIndex[index2] = originIndex2;

			currentVertexID += 3;
		}
		mesh.SetPolygon(faceIndex, cd::VertexID(index0), cd::VertexID(index1), cd::VertexID(index2));
	}

	cd::VertexFormat meshVertexFormat;
	assert(pSourceMesh->HasPositions() && "Mesh doesn't have vertex positions.");
	for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
	{
		uint32_t vertexDataIndex = vertexIndex;
		if (IsDuplicateVertexServiceActive())
		{
			auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
			assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
			vertexDataIndex = itNewIndex->second;
		}

		const aiVector3D& position = pSourceMesh->mVertices[vertexDataIndex];
		mesh.SetVertexPosition(vertexIndex, cd::Point(position.x, position.y, position.z));
	}
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);

	if (pSourceMesh->HasNormals())
	{
		for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
		{
			uint32_t vertexDataIndex = vertexIndex;
			if (IsDuplicateVertexServiceActive())
			{
				auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
				assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
				vertexDataIndex = itNewIndex->second;
			}

			const aiVector3D& normal = pSourceMesh->mNormals[vertexDataIndex];
			mesh.SetVertexNormal(vertexIndex, cd::Direction(normal.x, normal.y, normal.z));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);

		if (pSourceMesh->HasTangentsAndBitangents())
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (IsDuplicateVertexServiceActive())
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& tangent = pSourceMesh->mTangents[vertexDataIndex];
				mesh.SetVertexTangent(vertexIndex, cd::Direction(tangent.x, tangent.y, tangent.z));
			}
			meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);


			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (IsDuplicateVertexServiceActive())
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

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
			if (IsDuplicateVertexServiceActive())
			{
				auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
				assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
				vertexDataIndex = itNewIndex->second;
			}

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
			if (IsDuplicateVertexServiceActive())
			{
				auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
				assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
				vertexDataIndex = itNewIndex->second;
			}

			const aiColor4D& color = vertexColorArray[vertexDataIndex];
			mesh.SetVertexColor(colorSetIndex, vertexIndex, cd::Color(color.r, color.g, color.b, color.a));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Color::ValueType>(), cd::Color::Size);
	}

	mesh.SetVertexFormat(cd::MoveTemp(meshVertexFormat));
	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
	return meshID;
}

cd::TransformID GenericProducerImpl::AddTransform(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t transformID)
{
	aiMatrix4x4 sourceMatrix = pSourceNode->mTransformation;
	cd::Matrix4x4 transformation(sourceMatrix.a1, sourceMatrix.b1, sourceMatrix.c1, sourceMatrix.d1,
		sourceMatrix.a2, sourceMatrix.b2, sourceMatrix.c2, sourceMatrix.d2,
		sourceMatrix.a3, sourceMatrix.b3, sourceMatrix.c3, sourceMatrix.d3,
		sourceMatrix.a4, sourceMatrix.b4, sourceMatrix.c4, sourceMatrix.d4);

	cd::TransformID sceneTransformID(transformID);
	cd::Transform transform(sceneTransformID, cd::MoveTemp(transformation));

	// Add meshes from transform node.
	for (uint32_t meshIndex = 0; meshIndex < pSourceNode->mNumMeshes; ++meshIndex)
	{
		uint32_t sceneMeshIndex = pSourceNode->mMeshes[meshIndex];
		cd::MeshID meshID = AddMesh(pSceneDatabase, pSourceScene->mMeshes[sceneMeshIndex]);
		transform.AddMeshID(meshID.Data());
	}

	std::vector<uint32_t> childTransformIDs;
	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		uint32_t childTransformID = m_transformIDGenerator.AllocateID();
		childTransformIDs.push_back(childTransformID);
	}

	for (uint32_t childIndex = 0U; childIndex < pSourceNode->mNumChildren; ++childIndex)
	{
		uint32_t childTransformID = childTransformIDs[childIndex];
		transform.AddChildID(childTransformID);
		AddTransform(pSceneDatabase, pSourceScene, pSourceNode->mChildren[childIndex], childTransformID);
	}

	pSceneDatabase->AddTransform(cd::MoveTemp(transform));
	return sceneTransformID;
}

void GenericProducerImpl::SetSceneDatabaseIDs(uint32_t transformID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID)
{
	m_transformIDGenerator.SetCurrentID(transformID);
	m_meshIDGenerator.SetCurrentID(meshID);
	m_materialIDGenerator.SetCurrentID(materialID);
	m_textureIDGenerator.SetCurrentID(textureID);
	m_lightIDGenerator.SetCurrentID(lightID);
}

void GenericProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	printf("ImportStaticMesh : %s\n", m_filePath.c_str());
	const aiScene* pScene = aiImportFile(m_filePath.c_str(), GetImportFlags());
	if (!pScene || !pScene->HasMeshes())
	{
		printf(aiGetErrorString());
		return;
	}
	assert(pScene->mNumTextures == 0 && "[Unsupported] parse embedded textures.");

	pSceneDatabase->SetName(m_filePath.c_str());

	// Process all transforms.
	aiNode* pSceneRootNode = pScene->mRootNode;
	pSceneDatabase->SetTransformCount(GetSceneNodesCount(pSceneRootNode));
	pSceneDatabase->SetMeshCount(pScene->mNumMeshes);
	AddTransform(pSceneDatabase, pScene, pSceneRootNode, m_transformIDGenerator.AllocateID());

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
		sceneAABB.Expand(mesh.GetAABB());

		// Query mesh used material indexes.
		if (optUsedMaterialIndexes.has_value())
		{
			optUsedMaterialIndexes.value().insert(mesh.GetMaterialID().Data());
		}
	}
	pSceneDatabase->SetAABB(cd::MoveTemp(sceneAABB));

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

	printf("Scene mesh number : %d\n", pSceneDatabase->GetMeshCount());
	printf("Scene material number : %d\n", pSceneDatabase->GetMaterialCount());
	printf("Scene texture number : %d\n", pSceneDatabase->GetTextureCount());

	aiReleaseImport(pScene);
	pScene = nullptr;
}

}