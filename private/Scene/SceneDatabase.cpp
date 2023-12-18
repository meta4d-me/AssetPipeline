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

PIMPL_SIMPLE_TYPE_APIS(SceneDatabase, Unit);
PIMPL_COMPLEX_TYPE_APIS(SceneDatabase, AABB);
PIMPL_COMPLEX_TYPE_APIS(SceneDatabase, AxisSystem);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Animation);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Bone);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Camera);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Light);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Material);
PIMPL_VECTOR_TYPE_APIS_WITH_PLURAL(SceneDatabase, Mesh, es);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Morph);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Node);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, ParticleEmitter);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Skeleton);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Skin);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Texture);
PIMPL_VECTOR_TYPE_APIS(SceneDatabase, Track);
PIMPL_STRING_TYPE_APIS(SceneDatabase, Name);

///////////////////////////////////////////////////////////////////
// Bone
///////////////////////////////////////////////////////////////////
Bone* SceneDatabase::GetBoneByName(const char* pName)
{
	return m_pSceneDatabaseImpl->GetBoneByName(pName);
}

const Bone* SceneDatabase::GetBoneByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetBoneByName(pName);
}

///////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////
Node* SceneDatabase::GetNodeByName(const char* pName)
{
	return m_pSceneDatabaseImpl->GetNodeByName(pName);
}

const Node* SceneDatabase::GetNodeByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetNodeByName(pName);
}

///////////////////////////////////////////////////////////////////
// Track
///////////////////////////////////////////////////////////////////
Track* SceneDatabase::GetTrackByName(const char* pName)
{
	return m_pSceneDatabaseImpl->GetTrackByName(pName);
}

const Track* SceneDatabase::GetTrackByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetTrackByName(pName);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void SceneDatabase::Dump() const
{
	m_pSceneDatabaseImpl->Dump();
}

void SceneDatabase::Validate() const
{
	m_pSceneDatabaseImpl->Validate();
}

void SceneDatabase::Merge(cd::SceneDatabase&& scene)
{
	m_pSceneDatabaseImpl->Merge(cd::MoveTemp(*scene.m_pSceneDatabaseImpl));
}

void SceneDatabase::UpdateAABB()
{
	m_pSceneDatabaseImpl->UpdateAABB();
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