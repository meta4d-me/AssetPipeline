#include "SceneDatabase.h"

#include <cassert>

namespace cdtools
{

void SceneDatabase::SetName(std::string sceneName)
{
	m_name = std::move(sceneName);
}

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabase::SetMeshCount(uint32_t meshCount)
{
	m_meshes.reserve(meshCount);
}

void SceneDatabase::AddMesh(Mesh mesh)
{
	m_meshes.emplace_back(std::move(mesh));
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabase::SetMaterialCount(uint32_t materialCount)
{
	m_materials.reserve(materialCount);
}

void SceneDatabase::AddMaterial(Material material)
{
	m_materials.emplace_back(std::move(material));
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabase::SetTextureCount(uint32_t textureCount)
{
	m_textures.reserve(textureCount);
}

std::optional<TextureID> SceneDatabase::TryGetTextureID(const char* pTexturePath) const
{
	TextureMap::const_iterator itTexture = m_mapPathToTextureIDs.find(pTexturePath);
	if(itTexture != m_mapPathToTextureIDs.end())
	{
		return itTexture->second;
	}

	return std::nullopt;
}

void SceneDatabase::AddTexture(Texture texture)
{
	assert(m_mapPathToTextureIDs.find(texture.GetPath()) == m_mapPathToTextureIDs.end() && "Duplicated texture!");
	m_mapPathToTextureIDs[texture.GetPath()] = texture.GetID();
	m_textures.emplace_back(std::move(texture));
}

}