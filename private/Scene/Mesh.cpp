#include "Scene/Mesh.h"
#include "MeshImpl.h"

#include <cassert>

namespace cd
{

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
	m_pMeshImpl = new MeshImpl(vertexCount, polygonCount);
}

Mesh::Mesh(MeshID meshID, const char* pMeshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_pMeshImpl = new MeshImpl(meshID, pMeshName, vertexCount, polygonCount);
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

void Mesh::Init(MeshID meshID, const char* pMeshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_pMeshImpl->Init(meshID, pMeshName, vertexCount, polygonCount);
}

const MeshID& Mesh::GetID() const
{
	return m_pMeshImpl->GetID();
}

const char* Mesh::GetName() const
{
	return m_pMeshImpl->GetName().c_str();
}

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

void Mesh::SetMaterialID(uint32_t materialIndex)
{
	return m_pMeshImpl->SetMaterialID(materialIndex);
}

const MaterialID& Mesh::GetMaterialID() const
{
	return m_pMeshImpl->GetMaterialID();
}

//////////////////////////////////////////////////////////////////////////
// Vertex geometry data
//////////////////////////////////////////////////////////////////////////
void Mesh::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_pMeshImpl->SetVertexPosition(vertexIndex, position);
}

std::vector<Point>& Mesh::GetVertexPositions()
{
	return m_pMeshImpl->GetVertexPositions();
}

Point& Mesh::GetVertexPosition(uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexPosition(vertexIndex);
}

const Point& Mesh::GetVertexPosition(uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexPosition(vertexIndex);
}

const std::vector<Point>& Mesh::GetVertexPositions() const
{
	return m_pMeshImpl->GetVertexPositions();
}

void Mesh::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_pMeshImpl->SetVertexNormal(vertexIndex, normal);
}

std::vector<Direction>& Mesh::GetVertexNormals()
{
	return m_pMeshImpl->GetVertexNormals();
}

Direction& Mesh::GetVertexNormal(uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexNormal(vertexIndex);
}

const Direction& Mesh::GetVertexNormal(uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexNormal(vertexIndex);
}

const std::vector<Direction>& Mesh::GetVertexNormals() const
{
	return m_pMeshImpl->GetVertexNormals();
}

void Mesh::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_pMeshImpl->SetVertexTangent(vertexIndex, tangent);
}

std::vector<Direction>& Mesh::GetVertexTangents()
{
	return m_pMeshImpl->GetVertexTangents();
}

Direction& Mesh::GetVertexTangent(uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexTangent(vertexIndex);
}

const Direction& Mesh::GetVertexTangent(uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexTangent(vertexIndex);
}

const std::vector<Direction>& Mesh::GetVertexTangents() const
{
	return m_pMeshImpl->GetVertexTangents();
}

void Mesh::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_pMeshImpl->SetVertexBiTangent(vertexIndex, biTangent);
}

std::vector<Direction>& Mesh::GetVertexBiTangents()
{
	return m_pMeshImpl->GetVertexBiTangents();
}

Direction& Mesh::GetVertexBiTangent(uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexBiTangent(vertexIndex);
}

const Direction& Mesh::GetVertexBiTangent(uint32_t vertexIndex) const
{
	return m_pMeshImpl->GetVertexBiTangent(vertexIndex);
}

const std::vector<Direction>& Mesh::GetVertexBiTangents() const
{
	return m_pMeshImpl->GetVertexBiTangents();
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

BoneID& Mesh::GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex)
{
	return m_pMeshImpl->GetVertexBoneID(boneIndex, vertexIndex);
}

const BoneID& Mesh::GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) const
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
void Mesh::SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2)
{
	m_pMeshImpl->SetPolygon(polygonIndex, v0, v1, v2);
}

std::vector<Polygon>& Mesh::GetPolygons()
{
	return m_pMeshImpl->GetPolygons();
}

const std::vector<Polygon>& Mesh::GetPolygons() const
{
	return m_pMeshImpl->GetPolygons();
}

const Polygon& Mesh::GetPolygon(uint32_t polygonIndex) const
{
	return m_pMeshImpl->GetPolygon(polygonIndex);
}

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