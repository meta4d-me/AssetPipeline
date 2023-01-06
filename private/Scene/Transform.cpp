#include "Scene/Transform.h"
#include "TransformImpl.h"

namespace cd
{

Transform::Transform(InputArchive& inputArchive)
{
    m_pTransformImpl = new TransformImpl(inputArchive);
}

Transform::Transform(InputArchiveSwapBytes& inputArchive)
{
    m_pTransformImpl = new TransformImpl(inputArchive);
}

Transform::Transform(TransformID transformID)
{
    m_pTransformImpl = new TransformImpl(transformID);
}

Transform::Transform(Transform&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Transform& Transform::operator=(Transform&& rhs)
{
    std::swap(m_pTransformImpl, rhs.m_pTransformImpl);
    return *this;
}

Transform::~Transform()
{
    if (m_pTransformImpl)
    {
        delete m_pTransformImpl;
        m_pTransformImpl = nullptr;
    }
}

void Transform::Init(TransformID transformID)
{
    m_pTransformImpl->Init(transformID);
}

const TransformID& Transform::GetID() const
{
    return m_pTransformImpl->GetID();
}

void Transform::SetParentID(uint32_t parentID)
{
    m_pTransformImpl->SetParentID(parentID);
}

const TransformID& Transform::GetParentID() const
{
    return m_pTransformImpl->GetParentID();
}

void Transform::AddChildID(uint32_t childID)
{
    m_pTransformImpl->AddChildID(childID);
}

uint32_t Transform::GetChildCount() const
{
    return m_pTransformImpl->GetChildCount();
}

std::vector<TransformID>& Transform::GetChildIDs()
{
    return m_pTransformImpl->GetChildIDs();
}

const std::vector<TransformID>& Transform::GetChildIDs() const
{
    return m_pTransformImpl->GetChildIDs();
}

void Transform::AddMeshID(uint32_t meshID)
{
    m_pTransformImpl->AddMeshID(meshID);
}

uint32_t Transform::GetMeshCount() const
{
    return m_pTransformImpl->GetMeshCount();
}

std::vector<MeshID>& Transform::GetMeshIDs()
{
    return m_pTransformImpl->GetMeshIDs();
}

const std::vector<MeshID>& Transform::GetMeshIDs() const
{
    return m_pTransformImpl->GetMeshIDs();
}

void Transform::SetTransformation(Matrix4x4 transformation)
{
    m_pTransformImpl->SetTransformation(cd::MoveTemp(transformation));
}

Matrix4x4& Transform::GetTransformation()
{
    return m_pTransformImpl->GetTransformation();
}

const Matrix4x4& Transform::GetTransformation() const
{
    return m_pTransformImpl->GetTransformation();
}

Transform& Transform::operator<<(InputArchive& inputArchive)
{
    *m_pTransformImpl << inputArchive;
    return *this;
}

Transform& Transform::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pTransformImpl << inputArchive;
    return *this;
}

const Transform& Transform::operator>>(OutputArchive& outputArchive) const
{
    *m_pTransformImpl >> outputArchive;
    return *this;
}

const Transform& Transform::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pTransformImpl >> outputArchive;
    return *this;
}

}