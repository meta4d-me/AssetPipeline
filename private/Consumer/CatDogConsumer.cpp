#include "CatDogConsumer.h"

#include "Scene/SceneDatabase.h"

// C/C++
#include <fstream>

namespace cdtools
{

void CatDogConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	std::ofstream fout(m_filePath, std::ios::out | std::ios::binary);

	size_t sceneNameLength = pSceneDatabase->GetName().size();
	fout.write(reinterpret_cast<char*>(&sceneNameLength), sizeof(sceneNameLength));
	fout.write(pSceneDatabase->GetName().c_str(), sceneNameLength);

	uint32_t meshCount = pSceneDatabase->GetMeshCount();
	uint32_t materialCount = pSceneDatabase->GetMaterialCount();
	uint32_t textureCount = pSceneDatabase->GetTextureCount();
	fout.write(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	fout.write(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
	fout.write(reinterpret_cast<char*>(&textureCount), sizeof(textureCount));

	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		const Mesh& mesh = pSceneDatabase->GetMesh(meshIndex);
		mesh.ExportBinary(fout);
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		const Texture& texture = pSceneDatabase->GetTexture(textureIndex);
		texture.ExportBinary(fout);
	}

	for(uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		const Material& material = pSceneDatabase->GetMaterial(materialIndex);
		material.ExportBinary(fout);
	}

	fout.close();
}

}