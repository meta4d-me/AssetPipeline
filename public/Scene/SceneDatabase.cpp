#include "SceneDatabase.h"

#include <cassert>

namespace cdtools
{

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

///////////////////////////////////////////////////////////////////
// Import/Export
///////////////////////////////////////////////////////////////////
void SceneDatabase::ImportBinary(std::ifstream& fin)
{
	std::string sceneName;
	ImportData(fin, sceneName);
	SetName(std::move(sceneName));

	AABB sceneAABB;
	ImportDataBuffer(fin, sceneAABB.Min().begin());
	ImportDataBuffer(fin, sceneAABB.Max().begin());
	SetAABB(std::move(sceneAABB));

	uint32_t meshCount = 0;
	uint32_t materialCount = 0;
	uint32_t textureCount = 0;
	ImportData(fin, meshCount);
	ImportData(fin, materialCount);
	ImportData(fin, textureCount);
	SetMeshCount(meshCount);
	SetMaterialCount(materialCount);
	SetTextureCount(textureCount);

	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		Mesh mesh(fin);
		AddMesh(std::move(mesh));
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		Texture texture(fin);
		AddTexture(std::move(texture));
	}

	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		Material material(fin);
		AddMaterial(std::move(material));
	}
}

void SceneDatabase::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::string>(fout, GetName());

	ExportDataBuffer(fout, GetAABB().Min().begin(), GetAABB().Min().size());
	ExportDataBuffer(fout, GetAABB().Max().begin(), GetAABB().Max().size());

	ExportData<uint32_t>(fout, GetMeshCount());
	ExportData<uint32_t>(fout, GetMaterialCount());
	ExportData<uint32_t>(fout, GetTextureCount());

	for (uint32_t meshIndex = 0; meshIndex < GetMeshCount(); ++meshIndex)
	{
		const Mesh& mesh = GetMesh(meshIndex);
		mesh.ExportBinary(fout);
	}

	for (uint32_t textureIndex = 0; textureIndex < GetTextureCount(); ++textureIndex)
	{
		const Texture& texture = GetTexture(textureIndex);
		texture.ExportBinary(fout);
	}

	for (uint32_t materialIndex = 0; materialIndex < GetMaterialCount(); ++materialIndex)
	{
		const Material& material = GetMaterial(materialIndex);
		material.ExportBinary(fout);
	}
}

}