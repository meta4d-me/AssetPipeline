#include "Scene/SceneDatabase.h"
#include "SceneDatabaseImpl.h"

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
// Transform
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddTransform(Transform transform)
{
	return m_pSceneDatabaseImpl->AddTransform(cd::MoveTemp(transform));
}

const std::vector<Transform>& SceneDatabase::GetTransforms() const
{
	return m_pSceneDatabaseImpl->GetTransforms();
}

void SceneDatabase::SetTransformCount(uint32_t transformCount)
{
	return m_pSceneDatabaseImpl->SetTransformCount(transformCount);
}

const Transform& SceneDatabase::GetTransform(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetTransform(index);
}

uint32_t SceneDatabase::GetTransformCount() const
{
	return m_pSceneDatabaseImpl->GetTransformCount();
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
// Light
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddLight(Light light)
{
	return m_pSceneDatabaseImpl->AddLight(cd::MoveTemp(light));
}

const std::vector<Light>& SceneDatabase::GetLights() const
{
	return m_pSceneDatabaseImpl->GetLights();
}

void SceneDatabase::SetLightCount(uint32_t lightCount)
{
	return m_pSceneDatabaseImpl->SetLightCount(lightCount);
}

const Light& SceneDatabase::GetLight(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetLight(index);
}

uint32_t SceneDatabase::GetLightCount() const
{
	return m_pSceneDatabaseImpl->GetLightCount();
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