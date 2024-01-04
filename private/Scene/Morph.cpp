#include "Scene/Morph.h"
#include "MorphImpl.h"

#include <cassert>

namespace cd
{

PIMPL_SCENE_CLASS(Morph);

uint32_t Morph::GetVertexCount() const
{
	return m_pMorphImpl->GetVertexCount();
}

PIMPL_SIMPLE_TYPE_APIS(Morph, ID);
PIMPL_SIMPLE_TYPE_APIS(Morph, BlendShapeID);
PIMPL_SIMPLE_TYPE_APIS(Morph, Weight);
PIMPL_STRING_TYPE_APIS(Morph, Name);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexSourceID);
PIMPL_VECTOR_TYPE_APIS(Morph, VertexPosition);

}