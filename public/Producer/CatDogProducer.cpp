#include "CatDogProducer.h"

#include "Scene/ObjectIDTypes.h"
#include "Scene/SceneDatabase.h"

// C/C++
#include <fstream>

namespace cdtools
{

CatDogProducer::CatDogProducer(std::string filePath) :
	m_filePath(std::move(filePath))
{
}

void CatDogProducer::Execute(SceneDatabase* pSceneDatabase)
{
	std::ifstream fin(m_filePath, std::ios::in | std::ios::binary);

	size_t sceneNameLength;
	fin.read(reinterpret_cast<char*>(&sceneNameLength), sizeof(sceneNameLength));

	std::string sceneName;
	sceneName.resize(sceneNameLength);
	fin.read(const_cast<char*>(sceneName.data()), sceneNameLength);
	pSceneDatabase->SetName(std::move(sceneName));

	uint32_t meshCount = 0;
	uint32_t materialCount = 0;
	uint32_t textureCount = 0;
	fin.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	fin.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
	fin.read(reinterpret_cast<char*>(&textureCount), sizeof(textureCount));
	pSceneDatabase->SetMeshCount(meshCount);
	pSceneDatabase->SetMaterialCount(materialCount);
	pSceneDatabase->SetTextureCount(textureCount);

	size_t meshNameLength;
	uint32_t meshID;
	uint32_t meshMaterialID;
	uint32_t vertexCount;
	uint32_t vertexUVSetCount;
	uint32_t vertexColorSetCount;
	uint32_t polygonCount;
	size_t bufferBytes;
	for(uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		std::string meshName;
		fin.read(reinterpret_cast<char*>(&meshNameLength), sizeof(meshNameLength));
		meshName.resize(meshNameLength);
		fin.read(const_cast<char*>(meshName.data()), meshNameLength);

		fin.read(reinterpret_cast<char*>(&meshID), sizeof(meshID));
		fin.read(reinterpret_cast<char*>(&meshMaterialID), sizeof(meshMaterialID));
		fin.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
		fin.read(reinterpret_cast<char*>(&vertexUVSetCount), sizeof(vertexUVSetCount));
		fin.read(reinterpret_cast<char*>(&vertexColorSetCount), sizeof(vertexColorSetCount));
		fin.read(reinterpret_cast<char*>(&polygonCount), sizeof(polygonCount));

		Mesh mesh(MeshID(meshID), std::move(meshName), vertexCount, polygonCount);
		mesh.SetMaterialID(meshMaterialID);
		mesh.SetVertexUVSetCount(vertexUVSetCount);
		mesh.SetVertexColorSetCount(vertexColorSetCount);

		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(mesh.GetVertexPositions().data()), bufferBytes);

		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(mesh.GetVertexNormals().data()), bufferBytes);

		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(mesh.GetVertexTangents().data()), bufferBytes);

		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(mesh.GetVertexBiTangents().data()), bufferBytes);

		for (uint32_t uvSetIndex = 0; uvSetIndex < vertexUVSetCount; ++uvSetIndex)
		{
			fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
			fin.read(reinterpret_cast<char*>(mesh.GetVertexUV(uvSetIndex).data()), bufferBytes);
		}
		
		for (uint32_t colorSetIndex = 0; colorSetIndex < vertexColorSetCount; ++colorSetIndex)
		{
			fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
			fin.read(reinterpret_cast<char*>(mesh.GetVertexColor(colorSetIndex).data()), bufferBytes);
		}

		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(mesh.GetPolygons().data()), bufferBytes);

		pSceneDatabase->AddMesh(std::move(mesh));
	}

	size_t texturePathLength;
	uint32_t textureID;
	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		std::string texturePath;
		fin.read(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
		texturePath.resize(texturePathLength);
		fin.read(const_cast<char*>(texturePath.data()), texturePathLength);

		fin.read(reinterpret_cast<char*>(&textureID), sizeof(textureID));

		Texture texture(TextureID(textureID), std::move(texturePath));
		pSceneDatabase->AddTexture(std::move(texture));
	}

	size_t materialNameLength;
	uint32_t materialID;
	size_t materialTextureCount;
	size_t textureType;
	uint32_t materialTextureID;
	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		std::string materialName;
		fin.read(reinterpret_cast<char*>(&materialNameLength), sizeof(materialNameLength));
		materialName.resize(materialNameLength);
		fin.read(const_cast<char*>(materialName.data()), materialNameLength);

		fin.read(reinterpret_cast<char*>(&materialID), sizeof(materialID));
		fin.read(reinterpret_cast<char*>(&materialTextureCount), sizeof(materialTextureCount));

		Material material(MaterialID(materialID), std::move(materialName));

		for (uint32_t textureIndex = 0; textureIndex < materialTextureCount; ++textureIndex)
		{
			fin.read(reinterpret_cast<char*>(&textureType), sizeof(textureType));
			fin.read(reinterpret_cast<char*>(&materialTextureID), sizeof(materialTextureID));
			material.SetTextureID(static_cast<MaterialTextureType>(textureType), TextureID(materialTextureID));
		}

		pSceneDatabase->AddMaterial(std::move(material));
	}

	fin.close();
}

}