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

PIMPL_SIMPLE_TYPE_APIS(Morph, ID);
PIMPL_SIMPLE_TYPE_APIS(Morph, SourceMeshID);
PIMPL_SIMPLE_TYPE_APIS(Morph, Weight);
PIMPL_SIMPLE_TYPE_APIS(Morph, VertexCount);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexSourceID);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexPosition);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexNormal);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexTangent);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexBiTangent);
PIMPL_STRING_TYPE_APIS(Morph, Name);

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