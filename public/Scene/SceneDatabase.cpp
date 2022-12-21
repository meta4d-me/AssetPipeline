#include "SceneDatabase.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
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
	m_meshes.emplace_back(MoveTemp(mesh));
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
	m_materials.emplace_back(MoveTemp(material));
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
	m_textures.emplace_back(MoveTemp(texture));
}

}