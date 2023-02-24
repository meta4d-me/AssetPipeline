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
    m_pBoneImpl->Init(id, cd::MoveTemp(name));
}

const BoneID& Bone::GetID() const
{
    return m_pBoneImpl->GetID();
}

void Bone::SetName(std::string name)
{
    return m_pBoneImpl->SetName(cd::MoveTemp(name));
}

const char* Bone::GetName() const
{
    return m_pBoneImpl->GetName().c_str();
}

void Bone::SetParentID(uint32_t parentID)
{
    m_pBoneImpl->SetParentID(parentID);
}

const BoneID& Bone::GetParentID() const
{
    return m_pBoneImpl->GetParentID();
}

void Bone::AddChildID(uint32_t childID)
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