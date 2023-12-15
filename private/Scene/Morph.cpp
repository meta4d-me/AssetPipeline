#include "Scene/Morph.h"
#include "MorphImpl.h"

#include <cassert>

namespace cd
{

PIMPL_SCENE_CLASS(Morph);

Morph::Morph(uint32_t vertexCount)
{
	m_pMorphImpl = new MorphImpl(vertexCount);
}

Morph::Morph(MorphID id, MeshID sourceMeshID, const char* pName, uint32_t vertexCount)
{
	m_pMorphImpl = new MorphImpl(id, sourceMeshID, pName, vertexCount);
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

}