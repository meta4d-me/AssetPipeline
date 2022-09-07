#include "GenericProducer.h"
#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/Utils.h"

// 3rdParty
//#define ASSIMP_DOUBLE_PRECISION
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/version.h>

#include <cassert>
#include <optional>

namespace
{

constexpr uint32_t BasicImportModelFlags[] =
{
	aiProcess_Triangulate,
	aiProcess_GenBoundingBoxes,			// Generate bounding boxes for every mesh in the scene.
	aiProcess_PreTransformVertices,
};

constexpr uint32_t AdvancedImportModelFlags[] =
{
	aiProcess_ImproveCacheLocality,		// Improve GPU vertex data cache miss rate. That is, ACMR.
};

// Speed first. Only open flags which are necessary.
constexpr uint32_t DefaultImportModelFlags = cdtools::array_sum(BasicImportModelFlags);

// Quality first. Open extra flags which will have benefits for engine rendering.
constexpr uint32_t ReleaseImportModelFlags = DefaultImportModelFlags + cdtools::array_sum(AdvancedImportModelFlags);

// Parameters
// all textures for PBR Metalness-Roughness workflow purpose.
constexpr aiTextureType ImportTextureTypes[] =
{
	 aiTextureType_NONE,
	 aiTextureType_DIFFUSE,
	 aiTextureType_SPECULAR,
	 aiTextureType_AMBIENT,
	 aiTextureType_EMISSIVE,
	 aiTextureType_HEIGHT,
	 aiTextureType_NORMALS,
	 aiTextureType_SHININESS,
	 aiTextureType_OPACITY,
	 aiTextureType_DISPLACEMENT,
	 aiTextureType_LIGHTMAP,
	 aiTextureType_REFLECTION,
	 aiTextureType_BASE_COLOR,
	 aiTextureType_NORMAL_CAMERA,
	 aiTextureType_EMISSION_COLOR,
	 aiTextureType_METALNESS,
	 aiTextureType_DIFFUSE_ROUGHNESS,
	 aiTextureType_AMBIENT_OCCLUSION,
	 aiTextureType_UNKNOWN,
};
constexpr uint32_t numImportTextureTypes = sizeof(ImportTextureTypes) / sizeof(aiTextureType);

}

namespace cdtools
{

GenericProducer::GenericProducer(std::string filePath) :
	m_filePath(std::move(filePath))
{
}

void GenericProducer::Execute(SceneDatabase* pSceneDatabase)
{
	constexpr bool doDuplicateVertices = true;
	// glTF file format
	//     - right hand coordinate system.
	//     - forward vector towards +z.
	//     - up vector towards +y.
	// bgfx geometryc tool
	//     - left hand coordinate system.
	//     - forward vector towards +z.
	//     - up vector towards +y.
	uint32_t glTFImportModelFlags[] =
	{
		aiProcess_ConvertToLeftHanded
	};
	uint32_t importFlags = DefaultImportModelFlags + array_sum(glTFImportModelFlags);

	// Run
	printf("ImportStaticMesh : %s\n", m_filePath.c_str());
	const aiScene* pScene = aiImportFile(m_filePath.c_str(), importFlags);
	if (!pScene || !pScene->HasMeshes())
	{
		printf(aiGetErrorString());
		return;
	}

	pSceneDatabase->SetName(m_filePath);

	printf("Scene embedded texture number : %d\n", pScene->mNumTextures);
	//assert(pScene->mNumTextures == 0 && "Don't support embedded texture now.");
	//for (uint32_t embeddedTextureIndex = 0; embeddedTextureIndex < pScene->mNumTextures; ++embeddedTextureIndex)
	//{
	//	const aiTexture* pEmbeddedTexture = pScene->mTextures[embeddedTextureIndex];
	//	assert(pEmbeddedTexture && "tjj : pTexture is invalid");
	//	pEmbeddedTexture->mFilename.data;
	//}

	aiString ai_path;
	uint32_t totalTextureCount = 0;

	pSceneDatabase->SetMaterialCount(pScene->mNumMaterials);
	printf("Scene material number : %d\n", pScene->mNumMaterials);
	for (uint32_t materialIndex = 0; materialIndex < pScene->mNumMaterials; ++materialIndex)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[materialIndex];
		assert(pMaterial && "pMaterial is invalid");

		aiString materialName;
		if (aiReturn_SUCCESS == aiGetMaterialString(pMaterial, AI_MATKEY_NAME, &materialName))
		{
			printf("\tMaterial name is %s\n", materialName.data);
		}
		else
		{
			// No name ? We should name it by ourselves to identify it.
			materialName = "MaterialName_" + materialIndex;
		}

		Material material(MaterialID(materialIndex), materialName.C_Str());

		// Parse material properties
		int32_t blendFunction = 0;
		aiGetMaterialIntegerArray(pMaterial, AI_MATKEY_BLEND_FUNC, &blendFunction, 0);
		if (aiBlendMode_Default == blendFunction)
		{
			// SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
		}
		else if (aiBlendMode_Additive == blendFunction)
		{
			// SourceColor*1 + DestColor*1
		}

		for (uint32_t textureTypeIndex = 0; textureTypeIndex < numImportTextureTypes; ++textureTypeIndex)
		{
			const aiTextureType textureType = ImportTextureTypes[textureTypeIndex];
			const uint32_t textureCount = aiGetMaterialTextureCount(pMaterial, textureType);
			if (0 == textureCount)
			{
				continue;
			}

			for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
			{
				const aiReturn result = aiGetMaterialTexture(pMaterial, textureType, textureIndex, &ai_path);
				if (aiReturn_SUCCESS == result)
				{
					printf("\t\tTextureType is %u, MaterialTexture path is %s\n", textureType, ai_path.C_Str());
					MaterialTextureType materialTextureType;
					if (aiTextureType_DIFFUSE == textureType)
					{
						materialTextureType = MaterialTextureType::BaseColor;
					}
					else if (aiTextureType_NORMALS == textureType)
					{
						materialTextureType = MaterialTextureType::Normal;
					}
					else if (aiTextureType_UNKNOWN == textureType)
					{
						materialTextureType = MaterialTextureType::Unknown;
					}
					else
					{
						materialTextureType = MaterialTextureType::Unknown;
					}

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
				else
				{
					printf("\t\tFailed to read material texture property, textureType is %u, texture index is %u", textureType, textureIndex);
				}
			}
		}

		pSceneDatabase->AddMaterial(std::move(material));
	}

	pSceneDatabase->SetMeshCount(pScene->mNumMeshes);
	printf("Scene mesh number : %d\n", pScene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex)
	{
		// Check if mesh is valid.
		// TODO : need to check degenerate triangles.
		const aiMesh* pMesh = pScene->mMeshes[meshIndex];
		assert(pMesh && "pMesh is invalid");

		printf("\tMesh face number : %d\n", pMesh->mNumFaces);
		assert(pMesh->mFaces && pMesh->mNumFaces > 0 && "No polygon data.");

		uint32_t numVertices = pMesh->mNumVertices;
		if (doDuplicateVertices)
		{
			numVertices = pMesh->mNumFaces * 3;
		}

		printf("\tMesh vertex number : %d\n", numVertices);
		assert(pMesh->mVertices && numVertices > 0 && "No vertex data.");

		// Start to fill mesh data structure
		Mesh mesh(MeshID(meshIndex), pMesh->mName.C_Str(), numVertices, pMesh->mNumFaces);

		// Or we can optimize these two variables by using template specialization
		std::map<uint32_t, uint32_t> mapNewIndexToOriginIndex;
		uint32_t currentVertexID = 0U;
		for (uint32_t faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex)
		{
			const aiFace& face = pMesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3 && "tjj : Do you forget to open importer's triangulate flag?");

			uint32_t originIndex0 = face.mIndices[0];
			uint32_t originIndex1 = face.mIndices[1];
			uint32_t originIndex2 = face.mIndices[2];

			uint32_t index0 = originIndex0;
			uint32_t index1 = originIndex1;
			uint32_t index2 = originIndex2;
			if (doDuplicateVertices)
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

		UV tempUV;
		Color tempColor;
		Point tempPoint;
		Direction tempDirection;

		if (pMesh->mVertices)
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& position = pMesh->mVertices[vertexDataIndex];
				tempPoint.Set(position.x, position.y, position.z);
				mesh.SetVertexPosition(vertexIndex, tempPoint);
			}
		}

		// bounding box
		pMesh->mAABB.mMin.x;
		pMesh->mAABB.mMin.y;
		pMesh->mAABB.mMin.z;
		pMesh->mAABB.mMax.x;
		pMesh->mAABB.mMax.y;
		pMesh->mAABB.mMax.z;

		if (pMesh->mNormals)
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& normal = pMesh->mNormals[vertexDataIndex];
				tempDirection.Set(normal.x, normal.y, normal.z);
				mesh.SetVertexNormal(vertexIndex, tempDirection);
			}
		}

		if (pMesh->mTangents)
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& tangent = pMesh->mTangents[vertexDataIndex];
				tempDirection.Set(tangent.x, tangent.y, tangent.z);
				mesh.SetVertexTangent(vertexIndex, tempDirection);
			}
		}

		if (pMesh->mBitangents)
		{
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& biTangent = pMesh->mBitangents[vertexDataIndex];
				tempDirection.Set(biTangent.x, biTangent.y, biTangent.z);
				mesh.SetVertexBiTangent(vertexIndex, tempDirection);
			}
		}

		uint32_t uvSetCount = 0;
		for (uint32_t uvSetIndex = 0; uvSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++uvSetIndex)
		{
			const aiVector3D* vertexUVArray = pMesh->mTextureCoords[uvSetIndex];
			if (!vertexUVArray)
			{
				break;
			}

			++uvSetCount;
		}
		assert(uvSetCount == pMesh->GetNumUVChannels());
		mesh.SetVertexUVSetCount(uvSetCount);

		for (uint32_t uvSetIndex = 0; uvSetIndex < uvSetCount; ++uvSetIndex)
		{
			const aiVector3D* vertexUVArray = pMesh->mTextureCoords[uvSetIndex];
			uint32_t numUVComponents = pMesh->mNumUVComponents[uvSetIndex];
			if (0 == numUVComponents)
			{
				// 2 means normal 2D texture
				// 3 means 3D texture such as cubemap
				continue;
			}

			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiVector3D& uv = vertexUVArray[vertexDataIndex];
				tempUV.Set(uv.x, uv.y);
				mesh.SetVertexUV(uvSetIndex, vertexIndex, tempUV);
			}
		}

		uint32_t colorSetCount = 0;
		for (uint32_t colorSetIndex = 0; colorSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS; ++colorSetIndex)
		{
			const aiColor4D* vertexColorArray = pMesh->mColors[colorSetIndex];
			if (!vertexColorArray)
			{
				break;
			}

			++colorSetCount;
		}
		assert(colorSetCount == pMesh->GetNumColorChannels());
		mesh.SetVertexColorSetCount(colorSetCount);

		for (uint32_t colorSetIndex = 0; colorSetIndex < colorSetCount; ++colorSetIndex)
		{
			const aiColor4D* vertexColorArray = pMesh->mColors[colorSetIndex];
			for (uint32_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
			{
				uint32_t vertexDataIndex = vertexIndex;
				if (doDuplicateVertices)
				{
					auto itNewIndex = mapNewIndexToOriginIndex.find(vertexIndex);
					assert(itNewIndex != mapNewIndexToOriginIndex.end() && "Cannot find origin vertex index.");
					vertexDataIndex = itNewIndex->second;
				}

				const aiColor4D& color = vertexColorArray[vertexDataIndex];
				tempColor.Set(color.r, color.g, color.b, color.a);
				mesh.SetVertexColor(colorSetIndex, vertexIndex, tempColor);
			}
		}

		mesh.SetMaterialID(pMesh->mMaterialIndex);

		pSceneDatabase->AddMesh(std::move(mesh));
	}

	aiReleaseImport(pScene);
	pScene = nullptr;
}

}