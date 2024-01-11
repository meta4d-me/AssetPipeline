#include "Scene/Skin.h"
#include "SkinImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Skin);
PIMPL_SIMPLE_TYPE_APIS(Skin, ID);
PIMPL_SIMPLE_TYPE_APIS(Skin, MeshID);
PIMPL_SIMPLE_TYPE_APIS(Skin, SkeletonID);
PIMPL_SIMPLE_TYPE_APIS(Skin, MaxVertexInfluenceCount);
PIMPL_STRING_TYPE_APIS(Skin, Name);
PIMPL_VECTOR_TYPE_APIS(Skin, InfluenceBoneName);
PIMPL_VECTOR_TYPE_APIS(Skin, VertexBoneNameArray);
PIMPL_VECTOR_TYPE_APIS(Skin, VertexBoneWeightArray);

}