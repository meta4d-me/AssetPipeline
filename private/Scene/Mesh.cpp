#include "Scene/Mesh.h"
#include "MeshImpl.h"

#include <cassert>

namespace cd
{

PIMPL_SCENE_CLASS(Mesh);

PIMPL_SIMPLE_TYPE_APIS(Mesh, ID);
PIMPL_STRING_TYPE_APIS(Mesh, Name);
PIMPL_COMPLEX_TYPE_APIS(Mesh, AABB);
PIMPL_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
PIMPL_VECTOR_TYPE_APIS(Mesh, MaterialID);
PIMPL_VECTOR_TYPE_APIS(Mesh, BlendShapeID);
PIMPL_VECTOR_TYPE_APIS(Mesh, SkinID);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexInstanceToID);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexIDToInstance);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexPosition);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexNormal);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexTangent);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
PIMPL_VECTOR_TYPE_APIS(Mesh, PolygonGroup);

Mesh Mesh::FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy)
{
	Mesh mesh;
	mesh.m_pMeshImpl = new MeshImpl();
	mesh.m_pMeshImpl->FromHalfEdgeMesh(halfEdgeMesh, strategy);
	return mesh;
}

void Mesh::Init(uint32_t vertexCount)
{
	m_pMeshImpl->Init(vertexCount);
}

void Mesh::Init(uint32_t vertexCount, uint32_t vertexInstanceCount)
{
	m_pMeshImpl->Init(vertexCount, vertexInstanceCount);
}

uint32_t Mesh::GetVertexCount() const
{
	return m_pMeshImpl->GetVertexCount();
}

uint32_t Mesh::GetPolygonCount() const
{
	return m_pMeshImpl->GetPolygonCount();
}

void Mesh::UpdateAABB()
{
	m_pMeshImpl->UpdateAABB();
}

//////////////////////////////////////////////////////////////////////////
// Vertex geometry data
//////////////////////////////////////////////////////////////////////////
void Mesh::ComputeVertexNormals()
{
	m_pMeshImpl->ComputeVertexNormals();
}

void Mesh::ComputeVertexTangents()
{
	m_pMeshImpl->ComputeVertexTangents();
}

//////////////////////////////////////////////////////////////////////////
// Vertex texturing data
//////////////////////////////////////////////////////////////////////////
void Mesh::SetVertexUVSetCount(uint32_t setCount)
{
	m_pMeshImpl->SetVertexUVSetCount(setCount);
}

uint32_t Mesh::GetVertexUVSetCount() const
{
	return m_pMeshImpl->GetVertexUVSetCount();
}

void Mesh::SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv)
{
	return m_pMeshImpl->SetVertexUV(setIndex, vertexIndex, uv);
}

std::vector<UV>& Mesh::GetVertexUVs(uint32_t uvSetIndex)
{
	return m_pMeshImpl->GetVertexUVs(uvSetIndex);
}

const std::vector<UV>& Mesh::GetVertexUV(uint32_t uvSetIndex) const
{
	return m_pMeshImpl->GetVertexUVs(uvSetIndex);
}

UV& Mesh::GetVertexUV(uint32_t setIndex, uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexUV(setIndex, vertexIndex);
}

const UV& Mesh::GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexUV(setIndex, vertexIndex);
}

void Mesh::SetVertexColorSetCount(uint32_t setCount)
{
	m_pMeshImpl->SetVertexColorSetCount(setCount);
}

uint32_t Mesh::GetVertexColorSetCount() const
{
	return m_pMeshImpl->GetVertexColorSetCount();
}

void Mesh::SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color)
{
	return m_pMeshImpl->SetVertexColor(setIndex, vertexIndex, color);
}

std::vector<Color>& Mesh::GetVertexColors(uint32_t colorSetIndex)
{
	return m_pMeshImpl->GetVertexColors(colorSetIndex);
}

const std::vector<Color>& Mesh::GetVertexColor(uint32_t colorSetIndex) const
{
	return m_pMeshImpl->GetVertexColors(colorSetIndex);
}

Color& Mesh::GetVertexColor(uint32_t setIndex, uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexColor(setIndex, vertexIndex);
}

const Color& Mesh::GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexColor(setIndex, vertexIndex);
}

}