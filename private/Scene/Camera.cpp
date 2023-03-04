#include "Scene/Camera.h"
#include "CameraImpl.h"

namespace cd
{

Camera::Camera(InputArchive& inputArchive)
{
    m_pCameraImpl = new CameraImpl(inputArchive);
}

Camera::Camera(InputArchiveSwapBytes& inputArchive)
{
    m_pCameraImpl = new CameraImpl(inputArchive);
}

Camera::Camera(CameraID id, std::string name)
{
    m_pCameraImpl = new CameraImpl(id, cd::MoveTemp(name));
}

Camera::Camera(Camera&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Camera& Camera::operator=(Camera&& rhs)
{
    std::swap(m_pCameraImpl, rhs.m_pCameraImpl);
    return *this;
}

Camera::~Camera()
{
    if (m_pCameraImpl)
    {
        delete m_pCameraImpl;
        m_pCameraImpl = nullptr;
    }
}

void Camera::Init(CameraID id, std::string name)
{
    m_pCameraImpl->Init(id, cd::MoveTemp(name));
}

const CameraID& Camera::GetID() const
{
    return m_pCameraImpl->GetID();
}

void Camera::SetName(std::string name)
{
    return m_pCameraImpl->SetName(cd::MoveTemp(name));
}

const char* Camera::GetName() const
{
    return m_pCameraImpl->GetName().c_str();
}

Camera& Camera::operator<<(InputArchive& inputArchive)
{
    *m_pCameraImpl << inputArchive;
    return *this;
}

Camera& Camera::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pCameraImpl << inputArchive;
    return *this;
}

const Camera& Camera::operator>>(OutputArchive& outputArchive) const
{
    *m_pCameraImpl >> outputArchive;
    return *this;
}

const Camera& Camera::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pCameraImpl >> outputArchive;
    return *this;
}

}