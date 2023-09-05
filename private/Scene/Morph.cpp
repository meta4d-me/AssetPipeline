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

Morph::Morph(MorphID id, MeshID sourceMeshID, const char* pName, uint32_t vertexCount)
{
	m_pMorphImpl = new MorphImpl(id, sourceMeshID, pName, vertexCount);
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

MeshID Morph::GetSourceMeshID() const
{
	return m_pMorphImpl->GetSourceMeshID();
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

PIMPL_ID_APIS(Morph);
PIMPL_NAME_APIS(Morph);
PIMPL_VECTOR_DATA_APIS(Morph, VertexSourceID);
PIMPL_VECTOR_DATA_APIS(Morph, VertexPosition);
PIMPL_VECTOR_DATA_APIS(Morph, VertexNormal);
PIMPL_VECTOR_DATA_APIS(Morph, VertexTangent);
PIMPL_VECTOR_DATA_APIS(Morph, VertexBiTangent);

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