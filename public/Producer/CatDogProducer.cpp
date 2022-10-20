#include "CatDogProducer.h"

#include "Scene/ObjectIDTypes.h"
#include "Scene/SceneDatabase.h"

// C/C++
#include <fstream>

namespace cdtools
{

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

	for(uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		Mesh mesh(fin);
		pSceneDatabase->AddMesh(std::move(mesh));
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		Texture texture(fin);
		pSceneDatabase->AddTexture(std::move(texture));
	}

	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		Material material(fin);
		pSceneDatabase->AddMaterial(std::move(material));
	}

	fin.close();
}

}