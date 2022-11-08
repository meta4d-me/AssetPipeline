#include "GenericProducer.h"
#include "Scene/SceneDatabase.h"
#include "../Utilities/Utils.h"

// 3rdParty
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cassert>
#include <optional>
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

void GenericProducer::ParseMaterial(SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial) const
{
	static uint32_t totalTextureCount = 0;
	static uint32_t totalMaterialCount = 0;

	// TODO : What should them map to ?
	// aiTextureType_SPECULAR,
	// aiTextureType_AMBIENT,
	// aiTextureType_HEIGHT,
	// aiTextureType_NORMALS,
	// aiTextureType_SHININESS,
	// aiTextureType_OPACITY,
	// aiTextureType_DISPLACEMENT,
	// aiTextureType_REFLECTION,
	std::unordered_map<aiTextureType, MaterialTextureType> materialTextureMapping;
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

	aiString materialName;
	if (aiReturn_SUCCESS == aiGetMaterialString(pSourceMaterial, AI_MATKEY_NAME, &materialName))
	{
		printf("\tMaterial name is %s\n", materialName.data);
	}
	else
	{
		// No name ? We should name it by ourselves to identify it.
		materialName = "MaterialName_" + totalMaterialCount;
	}

	Material material(MaterialID(totalMaterialCount++), materialName.C_Str());

	// Parse material properties
	int32_t blendFunction = 0;
	aiGetMaterialIntegerArray(pSourceMaterial, AI_MATKEY_BLEND_FUNC, &blendFunction, 0);
	if (aiBlendMode_Default == blendFunction)
	{
		// SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
	}
	else if (aiBlendMode_Additive == blendFunction)
	{
		// SourceColor*1 + DestColor*1
	}

	for (const auto& [textureType, materialTextureType] : materialTextureMapping)
	{
		// Multiple assimp texture types will map to the same texture to increase the successful rate.
		// So we will skip remain texture types once one texture type already setup successfully.
		if (material.IsTextureTypeSetup(materialTextureType))
		{
			continue;
		}

		const uint32_t textureCount = aiGetMaterialTextureCount(pSourceMaterial, textureType);
		if (0 == textureCount)
		{
			continue;
		}

		for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
		{
			aiString ai_path;
			const aiReturn result = aiGetMaterialTexture(pSourceMaterial, textureType, textureIndex, &ai_path);
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

			printf("\t\tTextureType is %s, MaterialTexture path is %s\n", GetMaterialTextureTypeName(materialTextureType), ai_path.C_Str());
			std::optional<TextureID> optTextureID = pSceneDatabase->TryGetTextureID(ai_path.C_Str());
			if (!optTextureID.has_value())
			{
				Texture texture(TextureID(totalTextureCount), ai_path.C_Str());
				optTextureID = texture.GetID();
				pSceneDatabase->AddTexture(std::move(texture));
				++totalTextureCount;
			}
			material.SetTextureID(materialTextureType, optTextureID.value());
		}
	}

	pSceneDatabase->AddMaterial(std::move(material));
}

void GenericProducer::ParseMesh(SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh) const
{
	static uint32_t totalMeshCount = 0;

	printf("\tMesh face number : %d\n", pSourceMesh->mNumFaces);
	assert(pSourceMesh->mFaces && pSourceMesh->mNumFaces > 0 && "No polygon data.");

	uint32_t numVertices = pSourceMesh->mNumVertices;
	if (IsDuplicateVertexServiceActive())
	{
		numVertices = pSourceMesh->mNumFaces * 3;
	}

	printf("\tMesh vertex number : %d\n", numVertices);
	assert(pSourceMesh->mVertices && numVertices > 0 && "No vertex data.");

	Mesh mesh(MeshID(totalMeshCount++), pSourceMesh->mName.C_Str(), numVertices, pSourceMesh->mNumFaces);
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
	}

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
	}

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

	pSceneDatabase->SetMaterialCount(pScene->mNumMaterials);
	printf("Scene material number : %d\n", pScene->mNumMaterials);
	for (uint32_t materialIndex = 0; materialIndex < pScene->mNumMaterials; ++materialIndex)
	{
		ParseMaterial(pSceneDatabase, pScene->mMaterials[materialIndex]);
	}

	pSceneDatabase->SetMeshCount(pScene->mNumMeshes);
	printf("Scene mesh number : %d\n", pScene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex)
	{
		ParseMesh(pSceneDatabase, pScene->mMeshes[meshIndex]);
	}

	// Merge a total AABB for all meshes in the scene.
	AABB sceneAABB;
	for (uint32_t meshIndex = 0; meshIndex < pSceneDatabase->GetMeshCount(); ++meshIndex)
	{
		sceneAABB.Expand(pSceneDatabase->GetMesh(meshIndex).GetAABB());
	}
	pSceneDatabase->SetAABB(std::move(sceneAABB));

	aiReleaseImport(pScene);
	pScene = nullptr;
}

}