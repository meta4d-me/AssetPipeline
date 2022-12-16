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

///////////////////////////////////////////////////////////////////
// Import/Export
///////////////////////////////////////////////////////////////////
SceneDatabase& SceneDatabase::operator<<(InputArchive& inputArchive)
{
	std::string sceneName;
	inputArchive >> sceneName;
	SetName(MoveTemp(sceneName));

	AABB sceneAABB;
	inputArchive.ImportBuffer(sceneAABB.Min().begin());
	inputArchive.ImportBuffer(sceneAABB.Max().begin());
	SetAABB(MoveTemp(sceneAABB));

	uint32_t meshCount = 0;
	uint32_t materialCount = 0;
	uint32_t textureCount = 0;
	inputArchive >> meshCount >> materialCount >> textureCount;
	SetMeshCount(meshCount);
	SetMaterialCount(materialCount);
	SetTextureCount(textureCount);

	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		AddMesh(Mesh(inputArchive));
	}

	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		AddTexture(Texture(inputArchive));
	}

	for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		AddMaterial(Material(inputArchive));
	}

	return *this;
}

}