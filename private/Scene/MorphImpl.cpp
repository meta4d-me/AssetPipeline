#include "MorphImpl.h"

#include <cassert>

namespace cd
{

MorphImpl::MorphImpl(uint32_t vertexCount)
{
	Init(vertexCount);
}

MorphImpl::MorphImpl(MorphID id, MeshID sourceMeshID, std::string name, uint32_t vertexCount) :
	m_name(MoveTemp(name)),
	m_id(id),
	m_sourceMeshID(sourceMeshID)
{
	Init(vertexCount);
}

void MorphImpl::Init(uint32_t vertexCount)
{
	assert(vertexCount > 0 && "No need to create an empty mesh.");

	m_vertexCount = vertexCount;

	m_vertexSourceIDs.resize(vertexCount);

	// pre-construct for attributes which almost all model files will have.
	m_vertexPositions.resize(vertexCount);
	m_vertexNormals.resize(vertexCount);
	m_vertexTangents.resize(vertexCount);
	m_vertexBiTangents.resize(vertexCount);
}

void MorphImpl::Init(MorphID id, std::string name, uint32_t vertexCount)
{
	m_id = id;
	m_name = MoveTemp(name);
	Init(vertexCount);
}

}