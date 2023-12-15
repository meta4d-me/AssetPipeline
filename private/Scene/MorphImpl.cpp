#include "MorphImpl.h"

#include <cassert>

namespace cd
{

MorphImpl::MorphImpl(uint32_t vertexCount)
{
	Init(vertexCount);
}

MorphImpl::MorphImpl(MorphID id, MeshID sourceMeshID, std::string name, uint32_t vertexCount) :
	m_Name(MoveTemp(name)),
	m_ID(id),
	m_SourceMeshID(sourceMeshID)
{
	Init(vertexCount);
}

void MorphImpl::Init(uint32_t vertexCount)
{
	assert(vertexCount > 0 && "No need to create an empty mesh.");

	SetVertexCount(vertexCount);

	GetVertexSourceIDs().resize(vertexCount);

	// pre-construct for attributes which almost all model files will have.
	GetVertexPositions().resize(vertexCount);
	GetVertexNormals().resize(vertexCount);
	GetVertexTangents().resize(vertexCount);
	GetVertexBiTangents().resize(vertexCount);
}

void MorphImpl::Init(MorphID id, std::string name, uint32_t vertexCount)
{
	SetID(id);
	SetName(MoveTemp(name));
	Init(vertexCount);
}

}