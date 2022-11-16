#include "GenericProducer.h"
#include "Hashers/StringHash.hpp"
#include "Scene/SceneDatabase.h"
#include "../Utilities/Utils.h"

// 3rdParty
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cassert>
#include <format>
#include <optional>
#include <set>
#include <unordered_map>

namespace cdtools
{

uint32_t GenericProducer::GetImportFlags() const
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

void GenericProducer::AddMaterial(SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial)
{
	static std::unordered_map<aiTextureType, MaterialTextureType> materialTextureMapping;
	{
		// Mapping assimp material key to pbr based material key.
		materialTextureMapping[aiTextureType_DIFFUSE] = MaterialTextureType::BaseColor;
		materialTextureMapping[aiTextureType_BASE_COLOR] = MaterialTextureType::BaseColor;
		materialTextureMapping[aiTextureType_NORMALS] = MaterialTextureType::Normal;
		materialTextureMapping[aiTextureType_NORMAL_CAMERA] = MaterialTextureType::Normal;
		materialTextureMapping[aiTextureType_EMISSIVE] = MaterialTextureType::Emissive;
		materialTextureMapping[aiTextureType_EMISSION_COLOR] = MaterialTextureType::Emissive;
		materialTextureMapping[aiTextureType_METALNESS] = MaterialTextureType::Metalness;
		materialTextureMapping[aiTextureType_DIFFUSE_ROUGHNESS] = MaterialTextureType::Roughness;
		materialTextureMapping[aiTextureType_AMBIENT_OCCLUSION] = MaterialTextureType::AO;
		materialTextureMapping[aiTextureType_LIGHTMAP] = MaterialTextureType::AO;
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
		finalMaterialName = std::format("untitled_{}", unnamedMaterialCount++);
		printf("\tWarning : current material doesn't have name?\n");
	}

	bool isMaterialReused;
	MaterialID::ValueType materialHash = StringHash<MaterialID::ValueType>(finalMaterialName);
	MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, isMaterialReused);
	if (isMaterialReused)
	{
		// Skip to parse material which should already exist in SceneDatabase.
		// Note that two materials with same configs but have different names will be considered as different materia.
		// Because we only use material name as hash value. Don't want to compare all parameters.
		return;
	}

	printf("\t[MaterialID %u] %s\n", materialID.Data(), finalMaterialName.c_str());
	Material material(materialID, finalMaterialName.c_str());

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
			uint32_t textureHash = StringHash<TextureID::ValueType>(ai_path.C_Str());
			TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, isTextureReused);
			printf("\t\t[TextureID %u] %s - %s\n",
				textureID.Data(),
				GetMaterialTextureTypeName(materialTextureType),
				ai_path.C_Str());

			material.SetTextureID(materialTextureType, textureID);

			// Reused textures don't need to add to SceneDatabase again.
			if (!isTextureReused)
			{
				pSceneDatabase->AddTexture(Texture(textureID, ai_path.C_Str()));
			}
		}
	}

	pSceneDatabase->AddMaterial(std::move(material));
}

void GenericProducer::AddMesh(SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh)
{
	assert(pSourceMesh->mFaces && pSourceMesh->mNumFaces > 0 && "No polygon data.");

	uint32_t numVertices = pSourceMesh->mNumVertices;
	if (IsDuplicateVertexServiceActive())
	{
		numVertices = pSourceMesh->mNumFaces * 3;
	}

	assert(pSourceMesh->mVertices && numVertices > 0 && "No vertex data.");

	MeshID meshID(pSceneDatabase->GetMeshCount());
	printf("\t[MeshID %u] face number : %u, vertex number : %u\n", meshID.Data(), pSourceMesh->mNumFaces, numVertices);

	Mesh mesh(meshID, pSourceMesh->mName.C_Str(), numVertices, pSourceMesh->mNumFaces);
	mesh.SetMaterialID(pSourceMesh->mMaterialIndex);

	// By default, aabb will be empty.
	if (IsBoundingBoxServiceActive())
	{
		AABB meshAABB(Point(pSourceMesh->mAABB.mMin.x, pSourceMesh->mAABB.mMin.y, pSourceMesh->mAABB.mMin.z),
			Point(pSourceMesh->mAABB.mMax.x, pSourceMesh->mAABB.mMax.y, pSourceMesh->mAABB.mMax.z));
		mesh.SetAABB(std::move(meshAABB));
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
		mesh.SetPolygon(faceIndex, VertexID(index0), VertexID(index1), VertexID(index2));
	}

	VertexFormat meshVertexFormat;
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
		mesh.SetVertexPosition(vertexIndex, Point(position.x, position.y, position.z));
	}
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::Position, GetAttributeValueType<Point::ValueType>(), 3);

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
			mesh.SetVertexNormal(vertexIndex, Direction(normal.x, normal.y, normal.z));
		}
		meshVertexFormat.AddAttributeLayout(VertexAttributeType::Normal, GetAttributeValueType<Direction::ValueType>(), 3);

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
				mesh.SetVertexTangent(vertexIndex, Direction(tangent.x, tangent.y, tangent.z));
			}
			meshVertexFormat.AddAttributeLayout(VertexAttributeType::Tangent, GetAttributeValueType<Direction::ValueType>(), 3);


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
				mesh.SetVertexBiTangent(vertexIndex, Direction(biTangent.x, biTangent.y, biTangent.z));
			}
			meshVertexFormat.AddAttributeLayout(VertexAttributeType::Bitangent, GetAttributeValueType<Direction::ValueType>(), 3);
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
			mesh.SetVertexUV(uvSetIndex, vertexIndex, UV(uv.x, uv.y));
		}
		meshVertexFormat.AddAttributeLayout(VertexAttributeType::UV, GetAttributeValueType<UV::ValueType>(), 2);
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
			mesh.SetVertexColor(colorSetIndex, vertexIndex, Color(color.r, color.g, color.b, color.a));
		}
		meshVertexFormat.AddAttributeLayout(VertexAttributeType::Color, GetAttributeValueType<Color::ValueType>(), 4);
	}

	mesh.SetVertexFormat(std::move(meshVertexFormat));
	pSceneDatabase->AddMesh(std::move(mesh));
}

void GenericProducer::Execute(SceneDatabase* pSceneDatabase)
{
	printf("ImportStaticMesh : %s\n", m_filePath.c_str());
	const aiScene* pScene = aiImportFile(m_filePath.c_str(), GetImportFlags());
	if (!pScene || !pScene->HasMeshes())
	{
		printf(aiGetErrorString());
		return;
	}
	assert(pScene->mNumTextures == 0 && "[Unsupported] parse embedded textures.");

	pSceneDatabase->SetName(m_filePath);

	// Process all meshes.
	pSceneDatabase->SetMeshCount(pScene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex)
	{
		AddMesh(pSceneDatabase, pScene->mMeshes[meshIndex]);
	}

	// Post-process meshes.
	AABB sceneAABB;
	std::optional<std::set<uint32_t>> optUsedMaterialIndexes = std::nullopt;
	if (IsCleanUnusedServiceActive())
	{
		optUsedMaterialIndexes = std::set<uint32_t>();
	}
	
	for (const Mesh& mesh : pSceneDatabase->GetMeshes())
	{
		// Merge a total AABB for all meshes in the scene.
		sceneAABB.Expand(mesh.GetAABB());

		// Query mesh used material indexes.
		if (optUsedMaterialIndexes.has_value())
		{
			optUsedMaterialIndexes.value().insert(mesh.GetMaterialID().Data());
		}
	}
	pSceneDatabase->SetAABB(std::move(sceneAABB));

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