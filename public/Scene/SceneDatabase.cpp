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

uint32_t SceneDatabase::GetNextMeshID() {
	// TODO make this thread-safe
	uint32_t next_id = m_next_mesh_id;
	++m_next_mesh_id;
	return next_id;
}

void SceneDatabase::AddMesh(Mesh mesh)
{
	m_meshes.emplace_back(cd::MoveTemp(mesh));
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
	m_materials.emplace_back(cd::MoveTemp(material));
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabase::SetTextureCount(uint32_t textureCount)
{
	m_textures.reserve(textureCount);
}

void SceneDatabase::AddTexture(Texture texture)
{
	m_textures.emplace_back(cd::MoveTemp(texture));
}

///////////////////////////////////////////////////////////////////
// Import/Export
///////////////////////////////////////////////////////////////////
void SceneDatabase::ImportBinary(std::ifstream& fin)
{
	std::string sceneName;
	ImportData(fin, sceneName);
	SetName(cd::MoveTemp(sceneName));

	AABB sceneAABB;
	ImportDataBuffer(fin, sceneAABB.Min().begin());
	ImportDataBuffer(fin, sceneAABB.Max().begin());
	SetAABB(cd::MoveTemp(sceneAABB));

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
		AddMesh(cd::MoveTemp(mesh));
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		Texture texture(fin);
		AddTexture(cd::MoveTemp(texture));
	}

	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		Material material(fin);
		AddMaterial(cd::MoveTemp(material));
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