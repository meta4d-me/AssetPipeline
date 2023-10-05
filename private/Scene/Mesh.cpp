#include "Scene/Mesh.h"
#include "MeshImpl.h"

#include <cassert>

namespace cd
{

Mesh Mesh::FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy)
{
	Mesh mesh;
	mesh.m_pMeshImpl = new MeshImpl();
	mesh.m_pMeshImpl->FromHalfEdgeMesh(halfEdgeMesh, strategy);
	return mesh;
}

Mesh::Mesh(InputArchive& inputArchive)
{
	m_pMeshImpl = new MeshImpl(inputArchive);
}

Mesh::Mesh(InputArchiveSwapBytes& inputArchive)
{
	m_pMeshImpl = new MeshImpl(inputArchive);
}

Mesh::Mesh(uint32_t vertexCount, uint32_t polygonCount)
{
	m_pMeshImpl = new MeshImpl();
	m_pMeshImpl->Init(vertexCount, polygonCount);
}

Mesh::Mesh(MeshID id, const char* pName, uint32_t vertexCount, uint32_t polygonCount) :
	Mesh(vertexCount, polygonCount)
{
	m_pMeshImpl->SetID(id);
	m_pMeshImpl->SetName(pName);
}

Mesh::Mesh(Mesh&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

Mesh& Mesh::operator=(Mesh&& rhs)
{
	std::swap(m_pMeshImpl, rhs.m_pMeshImpl);
	return *this;
}

Mesh::~Mesh()
{
	if (m_pMeshImpl)
	{
		delete m_pMeshImpl;
		m_pMeshImpl = nullptr;
	}
}

void Mesh::Init(uint32_t vertexCount, uint32_t polygonCount)
{
	m_pMeshImpl->Init(vertexCount, polygonCount);
}

PIMPL_ID_APIS(Mesh);
PIMPL_NAME_APIS(Mesh);
PIMPL_VECTOR_DATA_APIS(Mesh, MorphID);
PIMPL_VECTOR_DATA_APIS(Mesh, VertexPosition);
PIMPL_VECTOR_DATA_APIS(Mesh, VertexNormal);
PIMPL_VECTOR_DATA_APIS(Mesh, VertexTangent);
PIMPL_VECTOR_DATA_APIS(Mesh, VertexBiTangent);

uint32_t Mesh::GetVertexCount() const
{
	return m_pMeshImpl->GetVertexCount();
}

uint32_t Mesh::GetPolygonCount() const
{
	return m_pMeshImpl->GetPolygonCount();
}

void Mesh::SetVertexFormat(VertexFormat vertexFormat)
{
	return m_pMeshImpl->SetVertexFormat(cd::MoveTemp(vertexFormat));
}

VertexFormat& Mesh::GetVertexFormat()
{
	return m_pMeshImpl->GetVertexFormat();
}

const VertexFormat& Mesh::GetVertexFormat() const
{
	return m_pMeshImpl->GetVertexFormat();
}

void Mesh::SetAABB(AABB aabb)
{
	return m_pMeshImpl->SetAABB(cd::MoveTemp(aabb));
}

AABB& Mesh::GetAABB()
{
	return m_pMeshImpl->GetAABB();
}

const AABB& Mesh::GetAABB() const
{
	return m_pMeshImpl->GetAABB();
}

void Mesh::UpdateAABB()
{
	m_pMeshImpl->UpdateAABB();
}

void Mesh::SetMaterialID(MaterialID materialID)
{
	return m_pMeshImpl->SetMaterialID(materialID);
}

MaterialID Mesh::GetMaterialID() const
{
	return m_pMeshImpl->GetMaterialID();
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

//////////////////////////////////////////////////////////////////////////
// Polygon index data
//////////////////////////////////////////////////////////////////////////
void Mesh::SetPolygon(uint32_t polygonIndex, cd::Polygon polygon)
{
	m_pMeshImpl->SetPolygon(polygonIndex, cd::MoveTemp(polygon));
}

std::vector<Polygon>& Mesh::GetPolygons()
{
	return m_pMeshImpl->GetPolygons();
}

const std::vector<Polygon>& Mesh::GetPolygons() const
{
	return m_pMeshImpl->GetPolygons();
}

Polygon& Mesh::GetPolygon(uint32_t polygonIndex)
{
	return m_pMeshImpl->GetPolygon(polygonIndex);
}

const Polygon& Mesh::GetPolygon(uint32_t polygonIndex) const
{
	return m_pMeshImpl->GetPolygon(polygonIndex);
}

cd::VertexID Mesh::GetPolygonVertexID(uint32_t polygonIndex, uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetPolygonVertexID(polygonIndex, vertexIndex);
}

//////////////////////////////////////////////////////////////////////////
// Serialization
//////////////////////////////////////////////////////////////////////////
Mesh& Mesh::operator<<(InputArchive& inputArchive)
{
	*m_pMeshImpl << inputArchive;
	return *this;
}

Mesh& Mesh::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pMeshImpl << inputArchive;
	return *this;
}

const Mesh& Mesh::operator>>(OutputArchive& outputArchive) const
{
	*m_pMeshImpl >> outputArchive;
	return *this;
}

const Mesh& Mesh::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pMeshImpl >> outputArchive;
	return *this;
}

}