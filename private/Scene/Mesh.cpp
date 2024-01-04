#include "Scene/Mesh.h"
#include "MeshImpl.h"

#include <cassert>

namespace cd
{

PIMPL_SCENE_CLASS(Mesh);

Mesh Mesh::FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy)
{
	Mesh mesh;
	mesh.m_pMeshImpl = new MeshImpl();
	mesh.m_pMeshImpl->FromHalfEdgeMesh(halfEdgeMesh, strategy);
	return mesh;
}

Mesh::Mesh(uint32_t vertexCount)
{
	m_pMeshImpl = new MeshImpl();
	m_pMeshImpl->Init(vertexCount);
}

Mesh::Mesh(MeshID id, const char* pName, uint32_t vertexCount) :
	Mesh(vertexCount)
{
	m_pMeshImpl->SetID(id);
	m_pMeshImpl->SetName(pName);
}

void Mesh::Init(uint32_t vertexCount)
{
	m_pMeshImpl->Init(vertexCount);
}

PIMPL_SIMPLE_TYPE_APIS(Mesh, ID);
PIMPL_SIMPLE_TYPE_APIS(Mesh, BlendShapeID);
PIMPL_STRING_TYPE_APIS(Mesh, Name);
PIMPL_COMPLEX_TYPE_APIS(Mesh, AABB);
PIMPL_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
PIMPL_VECTOR_TYPE_APIS(Mesh, SkinID);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexInstanceID);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexPosition);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexNormal);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexTangent);
PIMPL_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
PIMPL_VECTOR_TYPE_APIS(Mesh, PolygonGroup);
PIMPL_VECTOR_TYPE_APIS(Mesh, MaterialID);

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

//////////////////////////////////////////////////////////////////////////
// Vertex animation data
//////////////////////////////////////////////////////////////////////////
void Mesh::SetVertexInfluenceCount(uint32_t influenceCount)
{
	m_pMeshImpl->SetVertexInfluenceCount(influenceCount);
}

uint32_t Mesh::GetVertexInfluenceCount() const
{
	return m_pMeshImpl->GetVertexInfluenceCount();
}

void Mesh::SetVertexBoneWeight(uint32_t boneIndex, uint32_t vertexIndex, BoneID boneID, VertexWeight weight)
{
	m_pMeshImpl->SetVertexBoneWeight(boneIndex, vertexIndex, boneID, weight);
}

std::vector<BoneID>& Mesh::GetVertexBoneIDs(uint32_t boneIndex)
{
	return m_pMeshImpl->GetVertexBoneIDs(boneIndex);
}

const std::vector<BoneID>& Mesh::GetVertexBoneIDs(uint32_t boneIndex) const
{
	return m_pMeshImpl->GetVertexBoneIDs(boneIndex);
}

BoneID Mesh::GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexBoneID(boneIndex, vertexIndex);
}

std::vector<VertexWeight>& Mesh::GetVertexWeights(uint32_t boneIndex)
{
	return m_pMeshImpl->GetVertexWeights(boneIndex);
}

const std::vector<VertexWeight>& Mesh::GetVertexWeights(uint32_t boneIndex) const
{
	return m_pMeshImpl->GetVertexWeights(boneIndex);
}

VertexWeight& Mesh::GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexWeight(boneIndex, vertexIndex);
}

const VertexWeight& Mesh::GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexWeight(boneIndex, vertexIndex);
}

}