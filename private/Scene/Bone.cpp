#include "Scene/Bone.h"
#include "BoneImpl.h"

namespace cd
{

Bone::Bone(InputArchive& inputArchive)
{
    m_pBoneImpl = new BoneImpl(inputArchive);
}

Bone::Bone(InputArchiveSwapBytes& inputArchive)
{
    m_pBoneImpl = new BoneImpl(inputArchive);
}

Bone::Bone(BoneID id, std::string name)
{
    m_pBoneImpl = new BoneImpl(id, cd::MoveTemp(name));
}

Bone::Bone(Bone&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Bone& Bone::operator=(Bone&& rhs)
{
    std::swap(m_pBoneImpl, rhs.m_pBoneImpl);
    return *this;
}

Bone::~Bone()
{
    if (m_pBoneImpl)
    {
        delete m_pBoneImpl;
        m_pBoneImpl = nullptr;
    }
}

void Bone::Init(BoneID id, std::string name)
{
    m_pBoneImpl->Init(id, MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Bone, ID);
PIMPL_SIMPLE_TYPE_APIS(Bone, ParentID);
PIMPL_COMPLEX_TYPE_APIS(Bone, Offset);
PIMPL_COMPLEX_TYPE_APIS(Bone, Transform);
PIMPL_VECTOR_TYPE_APIS(Bone, ChildID);
PIMPL_STRING_TYPE_APIS(Bone, Name);

Bone& Bone::operator<<(InputArchive& inputArchive)
{
    *m_pBoneImpl << inputArchive;
    return *this;
}

Bone& Bone::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pBoneImpl << inputArchive;
    return *this;
}

const Bone& Bone::operator>>(OutputArchive& outputArchive) const
{
    *m_pBoneImpl >> outputArchive;
    return *this;
}

const Bone& Bone::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pBoneImpl >> outputArchive;
    return *this;
}

}