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

PIMPL_ID_APIS(Bone);
PIMPL_NAME_APIS(Bone);

void Bone::SetParentID(BoneID parentID)
{
    m_pBoneImpl->SetParentID(parentID);
}

BoneID Bone::GetParentID() const
{
    return m_pBoneImpl->GetParentID();
}

void Bone::AddChildID(BoneID childID)
{
    m_pBoneImpl->AddChildID(childID);
}

uint32_t Bone::GetChildCount() const
{
    return m_pBoneImpl->GetChildCount();
}

std::vector<BoneID>& Bone::GetChildIDs()
{
    return m_pBoneImpl->GetChildIDs();
}

const std::vector<BoneID>& Bone::GetChildIDs() const
{
    return m_pBoneImpl->GetChildIDs();
}

void Bone::SetOffset(Matrix4x4 offset)
{
    m_pBoneImpl->SetOffset(MoveTemp(offset));
}

Matrix4x4& Bone::GetOffset()
{
    return m_pBoneImpl->GetOffset();
}

const Matrix4x4& Bone::GetOffset() const
{
    return m_pBoneImpl->GetOffset();
}

void Bone::SetTransform(Transform transform)
{
    return m_pBoneImpl->SetTransform(MoveTemp(transform));
}

Transform& Bone::GetTransform()
{
    return m_pBoneImpl->GetTransform();
}

const Transform& Bone::GetTransform() const
{
    return m_pBoneImpl->GetTransform();
}

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