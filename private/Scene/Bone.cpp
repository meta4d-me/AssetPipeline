#include "Scene/Bone.h"
#include "BoneImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Bone);

Bone::Bone(BoneID id, std::string name)
{
    m_pBoneImpl = new BoneImpl(id, cd::MoveTemp(name));
}

void Bone::Init(BoneID id, std::string name)
{
    m_pBoneImpl->Init(id, MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Bone, ID);
PIMPL_SIMPLE_TYPE_APIS(Bone, ParentID);
PIMPL_SIMPLE_TYPE_APIS(Bone, SkeletonID);
PIMPL_STRING_TYPE_APIS(Bone, Name);
PIMPL_COMPLEX_TYPE_APIS(Bone, Offset);
PIMPL_COMPLEX_TYPE_APIS(Bone, Transform);
PIMPL_VECTOR_TYPE_APIS(Bone, ChildID);

}