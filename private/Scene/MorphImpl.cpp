#include "MorphImpl.h"

#include <cassert>

namespace cd
{

MorphImpl::MorphImpl(uint32_t vertexCount)
{
	Init(vertexCount);
}

MorphImpl::MorphImpl(MorphID id, std::string name, uint32_t vertexCount) :
	m_id(id),
	m_name(MoveTemp(name))
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

void MorphImpl::SetVertexSourceID(uint32_t vertexIndex, uint32_t sourceID)
{
	m_vertexSourceIDs[vertexIndex] = cd::VertexID(sourceID);
}

////////////////////////////////////////////////////////////////////////////////////
// Vertex geometry data
////////////////////////////////////////////////////////////////////////////////////

void MorphImpl::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_vertexPositions[vertexIndex] = position;
}

void MorphImpl::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_vertexNormals[vertexIndex] = normal;
}

void MorphImpl::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_vertexTangents[vertexIndex] = tangent;
}

void MorphImpl::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_vertexBiTangents[vertexIndex] = biTangent;
}

}