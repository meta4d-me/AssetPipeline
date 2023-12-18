#include "Scene/Skeleton.h"
#include "SkeletonImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Skeleton);
PIMPL_SIMPLE_TYPE_APIS(Skeleton, ID);
PIMPL_SIMPLE_TYPE_APIS(Skeleton, RootBoneID);
PIMPL_VECTOR_TYPE_APIS(Skeleton, BoneID);
PIMPL_STRING_TYPE_APIS(Skeleton, Name);

}