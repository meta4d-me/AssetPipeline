#include "SceneDatabaseImpl.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
{

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMeshCount(uint32_t meshCount)
{
	m_meshes.reserve(meshCount);
}

void SceneDatabaseImpl::AddMesh(Mesh mesh)
{
	m_meshes.emplace_back(MoveTemp(mesh));
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMaterialCount(uint32_t materialCount)
{
	m_materials.reserve(materialCount);
}

void SceneDatabaseImpl::AddMaterial(Material material)
{
	m_materials.emplace_back(MoveTemp(material));
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetTextureCount(uint32_t textureCount)
{
	m_textures.reserve(textureCount);
}

void SceneDatabaseImpl::AddTexture(Texture texture)
{
	m_textures.emplace_back(MoveTemp(texture));
}

}