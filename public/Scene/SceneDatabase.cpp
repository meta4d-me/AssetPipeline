#include "SceneDatabase.h"
#include "SceneDatabaseImpl.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
{

SceneDatabase::SceneDatabase()
{
	m_pSceneDatabaseImpl = new SceneDatabaseImpl();
}

SceneDatabase::SceneDatabase(SceneDatabase&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

SceneDatabase& SceneDatabase::operator=(SceneDatabase&& rhs)
{
	std::swap(m_pSceneDatabaseImpl, rhs.m_pSceneDatabaseImpl);
	return *this;
}

SceneDatabase::~SceneDatabase()
{
	if (m_pSceneDatabaseImpl)
	{
		delete m_pSceneDatabaseImpl;
		m_pSceneDatabaseImpl = nullptr;
	}
}

void SceneDatabase::SetName(const char* pName)
{
	m_pSceneDatabaseImpl->SetName(pName);
}

const char* SceneDatabase::GetName() const
{
	return m_pSceneDatabaseImpl->GetName().c_str();
}

void SceneDatabase::SetAABB(AABB aabb)
{
	return m_pSceneDatabaseImpl->SetAABB(cd::MoveTemp(aabb));
}

AABB& SceneDatabase::GetAABB()
{
	return m_pSceneDatabaseImpl->GetAABB();
}

const AABB& SceneDatabase::GetAABB() const
{
	return m_pSceneDatabaseImpl->GetAABB();
}

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddMesh(Mesh mesh)
{
	return m_pSceneDatabaseImpl->AddMesh(cd::MoveTemp(mesh));
}

const std::vector<Mesh>& SceneDatabase::GetMeshes() const
{
	return m_pSceneDatabaseImpl->GetMeshes();
}

void SceneDatabase::SetMeshCount(uint32_t meshCount)
{
	return m_pSceneDatabaseImpl->SetMeshCount(meshCount);
}

const Mesh& SceneDatabase::GetMesh(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetMesh(index);
}

uint32_t SceneDatabase::GetMeshCount() const
{
	return m_pSceneDatabaseImpl->GetMeshCount();
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddMaterial(Material material)
{
	return m_pSceneDatabaseImpl->AddMaterial(cd::MoveTemp(material));
}

const std::vector<Material>& SceneDatabase::GetMaterials() const
{
	return m_pSceneDatabaseImpl->GetMaterials();
}

void SceneDatabase::SetMaterialCount(uint32_t materialCount)
{
	return m_pSceneDatabaseImpl->SetMaterialCount(materialCount);
}

const Material& SceneDatabase::GetMaterial(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetMaterial(index);
}

uint32_t SceneDatabase::GetMaterialCount() const
{
	return m_pSceneDatabaseImpl->GetMaterialCount();
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddTexture(Texture texture)
{
	return m_pSceneDatabaseImpl->AddTexture(cd::MoveTemp(texture));
}

const std::vector<Texture>& SceneDatabase::GetTextures() const
{
	return m_pSceneDatabaseImpl->GetTextures();
}

void SceneDatabase::SetTextureCount(uint32_t textureCount)
{
	return m_pSceneDatabaseImpl->SetTextureCount(textureCount);
}

const Texture& SceneDatabase::GetTexture(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetTexture(index);
}

uint32_t SceneDatabase::GetTextureCount() const
{
	return m_pSceneDatabaseImpl->GetTextureCount();
}

///////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////
SceneDatabase& SceneDatabase::operator<<(InputArchive& inputArchive)
{
	*m_pSceneDatabaseImpl << inputArchive;
	return *this;
}

SceneDatabase& SceneDatabase::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pSceneDatabaseImpl << inputArchive;
	return *this;
}

const SceneDatabase& SceneDatabase::operator>>(OutputArchive& outputArchive) const
{
	*m_pSceneDatabaseImpl >> outputArchive;
	return *this;
}

const SceneDatabase& SceneDatabase::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pSceneDatabaseImpl >> outputArchive;
	return *this;
}

}