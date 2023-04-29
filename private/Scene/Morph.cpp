#include "Scene/Morph.h"
#include "MorphImpl.h"

#include <cassert>

namespace cd
{

Morph::Morph(InputArchive& inputArchive)
{
	m_pMorphImpl = new MorphImpl(inputArchive);
}

Morph::Morph(InputArchiveSwapBytes& inputArchive)
{
	m_pMorphImpl = new MorphImpl(inputArchive);
}

Morph::Morph(uint32_t vertexCount)
{
	m_pMorphImpl = new MorphImpl(vertexCount);
}

Morph::Morph(MorphID id, const char* pName, uint32_t vertexCount)
{
	m_pMorphImpl = new MorphImpl(id, pName, vertexCount);
}

Morph::Morph(Morph&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

Morph& Morph::operator=(Morph&& rhs)
{
	std::swap(m_pMorphImpl, rhs.m_pMorphImpl);
	return *this;
}

Morph::~Morph()
{
	if (m_pMorphImpl)
	{
		delete m_pMorphImpl;
		m_pMorphImpl = nullptr;
	}
}

void Morph::Init(uint32_t vertexCount)
{
	m_pMorphImpl->Init(vertexCount);
}

void Morph::Init(MorphID id, const char* pName, uint32_t vertexCount)
{
	m_pMorphImpl->Init(id, pName, vertexCount);
}

MorphID Morph::GetID() const
{
	return m_pMorphImpl->GetID();
}

const char* Morph::GetName() const
{
	return m_pMorphImpl->GetName().c_str();
}

void Morph::SetWeight(float weight)
{
	m_pMorphImpl->SetWeight(weight);
}

float Morph::GetWeight() const
{
	return m_pMorphImpl->GetWeight();
}

uint32_t Morph::GetVertexCount() const
{
	return m_pMorphImpl->GetVertexCount();
}

void Morph::SetVertexSourceID(uint32_t vertexIndex, uint32_t sourceID)
{
	m_pMorphImpl->SetVertexSourceID(vertexIndex, sourceID);
}

VertexID Morph::GetVertexSourceID(uint32_t vertexIndex) const
{
	return m_pMorphImpl->GetVertexSourceID(vertexIndex);
}

std::vector<VertexID>& Morph::GetVertexSourceIDs()
{
	return m_pMorphImpl->GetVertexSourceIDs();
}

const std::vector<VertexID>& Morph::GetVertexSourceIDs() const
{
	return m_pMorphImpl->GetVertexSourceIDs();
}

//////////////////////////////////////////////////////////////////////////
// Vertex geometry data
//////////////////////////////////////////////////////////////////////////
void Morph::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_pMorphImpl->SetVertexPosition(vertexIndex, position);
}

Point& Morph::GetVertexPosition(uint32_t vertexIndex)
{
	return m_pMorphImpl->GetVertexPosition(vertexIndex);
}

const Point& Morph::GetVertexPosition(uint32_t vertexIndex) const
{
	return m_pMorphImpl->GetVertexPosition(vertexIndex);
}

std::vector<Point>& Morph::GetVertexPositions()
{
	return m_pMorphImpl->GetVertexPositions();
}

const std::vector<Point>& Morph::GetVertexPositions() const
{
	return m_pMorphImpl->GetVertexPositions();
}

void Morph::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_pMorphImpl->SetVertexNormal(vertexIndex, normal);
}

Direction& Morph::GetVertexNormal(uint32_t vertexIndex)
{
	return m_pMorphImpl->GetVertexNormal(vertexIndex);
}

const Direction& Morph::GetVertexNormal(uint32_t vertexIndex) const
{
	return m_pMorphImpl->GetVertexNormal(vertexIndex);
}

std::vector<Direction>& Morph::GetVertexNormals()
{
	return m_pMorphImpl->GetVertexNormals();
}

const std::vector<Direction>& Morph::GetVertexNormals() const
{
	return m_pMorphImpl->GetVertexNormals();
}

void Morph::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_pMorphImpl->SetVertexTangent(vertexIndex, tangent);
}

Direction& Morph::GetVertexTangent(uint32_t vertexIndex)
{
	return m_pMorphImpl->GetVertexTangent(vertexIndex);
}

const Direction& Morph::GetVertexTangent(uint32_t vertexIndex) const
{
	return m_pMorphImpl->GetVertexTangent(vertexIndex);
}

std::vector<Direction>& Morph::GetVertexTangents()
{
	return m_pMorphImpl->GetVertexTangents();
}

const std::vector<Direction>& Morph::GetVertexTangents() const
{
	return m_pMorphImpl->GetVertexTangents();
}

void Morph::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_pMorphImpl->SetVertexBiTangent(vertexIndex, biTangent);
}

Direction& Morph::GetVertexBiTangent(uint32_t vertexIndex)
{
	return m_pMorphImpl->GetVertexBiTangent(vertexIndex);
}

const Direction& Morph::GetVertexBiTangent(uint32_t vertexIndex) const
{
	return m_pMorphImpl->GetVertexBiTangent(vertexIndex);
}

std::vector<Direction>& Morph::GetVertexBiTangents()
{
	return m_pMorphImpl->GetVertexBiTangents();
}

const std::vector<Direction>& Morph::GetVertexBiTangents() const
{
	return m_pMorphImpl->GetVertexBiTangents();
}

Morph& Morph::operator<<(InputArchive& inputArchive)
{
	*m_pMorphImpl << inputArchive;
	return *this;
}

Morph& Morph::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pMorphImpl << inputArchive;
	return *this;
}

const Morph& Morph::operator>>(OutputArchive& outputArchive) const
{
	*m_pMorphImpl >> outputArchive;
	return *this;
}

const Morph& Morph::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pMorphImpl >> outputArchive;
	return *this;
}


}