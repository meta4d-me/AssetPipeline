#include "Scene/Skin.h"
#include "SkinImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Skin);
PIMPL_SIMPLE_TYPE_APIS(Skin, ID);
PIMPL_SIMPLE_TYPE_APIS(Skin, MeshID);
PIMPL_SIMPLE_TYPE_APIS(Skin, SkeletonID);
PIMPL_STRING_TYPE_APIS(Skin, Name);
PIMPL_VECTOR_TYPE_APIS(Skin, VertexInfluenceBoneID);
PIMPL_VECTOR_TYPE_APIS(Skin, VertexBoneIndex);
PIMPL_VECTOR_TYPE_APIS(Skin, VertexBoneWeight);

}