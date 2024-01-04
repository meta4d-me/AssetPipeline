#include "Scene/BlendShape.h"
#include "BlendShapeImpl.h"

#include <cassert>

namespace cd
{

PIMPL_SCENE_CLASS(BlendShape);

PIMPL_SIMPLE_TYPE_APIS(BlendShape, ID);
PIMPL_SIMPLE_TYPE_APIS(BlendShape, MeshID);
PIMPL_STRING_TYPE_APIS(BlendShape, Name);
PIMPL_VECTOR_TYPE_APIS(BlendShape, MorphID);

}