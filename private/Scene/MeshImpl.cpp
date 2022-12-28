#include "MeshImpl.h"

#include <cassert>

namespace cd
{

MeshImpl::MeshImpl(uint32_t vertexCount, uint32_t polygonCount)
{
	Init(vertexCount, polygonCount);
}

MeshImpl::MeshImpl(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount) :
	m_id(meshID),
	m_name(MoveTemp(meshName))
{
	Init(vertexCount, polygonCount);
}

void MeshImpl::Init(uint32_t vertexCount, uint32_t polygonCount)
{
	m_vertexCount = vertexCount;
	m_polygonCount = polygonCount;

	assert(vertexCount > 0 && "No need to create an empty mesh.");
	assert(polygonCount > 0 && "Expect to generate index buffer by ourselves?");

	// pre-construct for attributes which almost all model files will have.
	m_vertexPositions.resize(vertexCount);
	m_vertexNormals.resize(vertexCount);
	m_vertexTangents.resize(vertexCount);
	m_vertexBiTangents.resize(vertexCount);

	m_polygons.resize(polygonCount);
}

void MeshImpl::Init(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_id = meshID;
	m_name = MoveTemp(meshName);
	Init(vertexCount, polygonCount);
}

void MeshImpl::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_vertexPositions[vertexIndex] = position;
}

void MeshImpl::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_vertexNormals[vertexIndex] = normal;
}

void MeshImpl::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_vertexTangents[vertexIndex] = tangent;
}

void MeshImpl::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_vertexBiTangents[vertexIndex] = biTangent;
}

void MeshImpl::SetVertexUVSetCount(uint32_t setCount)
{
	m_vertexUVSetCount = setCount;
	for(uint32_t i = 0; i < m_vertexUVSetCount; ++i)
	{
		m_vertexUVSets[i].resize(m_vertexCount);
	}
}

void MeshImpl::SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv)
{
	m_vertexUVSets[setIndex][vertexIndex] = uv;
}

void MeshImpl::SetVertexColorSetCount(uint32_t setCount)
{
	m_vertexColorSetCount = setCount;
	for (uint32_t i = 0; i < m_vertexColorSetCount; ++i)
	{
		m_vertexColorSets[i].resize(m_vertexCount);
	}
}

void MeshImpl::SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color)
{
	m_vertexColorSets[setIndex][vertexIndex] = color;
}

void MeshImpl::SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2)
{
	m_polygons[polygonIndex][0] = v0;
	m_polygons[polygonIndex][1] = v1;
	m_polygons[polygonIndex][2] = v2;
}

}