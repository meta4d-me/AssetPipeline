#include "Scene/Bone.h"
#include "BoneImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Bone);

bool Bone::IsRootBone() const
{
    return m_pBoneImpl->IsRootBone();
}

PIMPL_SIMPLE_TYPE_APIS(Bone, ID);
PIMPL_SIMPLE_TYPE_APIS(Bone, ParentID);
PIMPL_SIMPLE_TYPE_APIS(Bone, SkeletonID);
PIMPL_SIMPLE_TYPE_APIS(Bone, LimbLength);
PIMPL_STRING_TYPE_APIS(Bone, Name);
PIMPL_COMPLEX_TYPE_APIS(Bone, LimbSize);
PIMPL_COMPLEX_TYPE_APIS(Bone, Offset);
PIMPL_COMPLEX_TYPE_APIS(Bone, Transform);
PIMPL_VECTOR_TYPE_APIS(Bone, ChildID);

}