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

Camera::Camera(CameraID id, const char* pName)
{
    m_pCameraImpl = new CameraImpl(id, pName);
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

PIMPL_ID_APIS(Camera);
PIMPL_NAME_APIS(Camera);

void Camera::SetEye(Vec3f eye)
{
    m_pCameraImpl->SetEye(cd::MoveTemp(eye));
}

Vec3f& Camera::GetEye()
{
    return m_pCameraImpl->GetEye();
}

const Vec3f& Camera::GetEye() const
{
    return m_pCameraImpl->GetEye();
}

void Camera::SetLookAt(Vec3f lookAt)
{
    m_pCameraImpl->SetLookAt(cd::MoveTemp(lookAt));
}

Vec3f& Camera::GetLookAt()
{
    return m_pCameraImpl->GetLookAt();
}

const Vec3f& Camera::GetLookAt() const
{
    return m_pCameraImpl->GetLookAt();
}

void Camera::SetUp(Vec3f up)
{
    m_pCameraImpl->SetUp(cd::MoveTemp(up));
}

Vec3f& Camera::GetUp()
{
    return m_pCameraImpl->GetUp();
}

const Vec3f& Camera::GetUp() const
{
    return m_pCameraImpl->GetUp();
}

void Camera::SetAspect(float aspect)
{
    m_pCameraImpl->SetAspect(aspect);
}

float& Camera::GetAspect()
{
    return m_pCameraImpl->GetAspect();
}

float Camera::GetAspect() const
{
    return m_pCameraImpl->GetAspect();
}

void Camera::SetFov(float fov)
{
    m_pCameraImpl->SetFov(fov);
}

float& Camera::GetFov()
{
    return m_pCameraImpl->GetFov();
}

float Camera::GetFov() const
{
    return m_pCameraImpl->GetFov();
}

void Camera::SetNearPlane(float near)
{
    m_pCameraImpl->SetNearPlane(near);
}

float& Camera::GetNearPlane()
{
    return m_pCameraImpl->GetNearPlane();
}

float Camera::GetNearPlane() const
{
    return m_pCameraImpl->GetNearPlane();
}

void Camera::SetFarPlane(float far)
{
    m_pCameraImpl->SetFarPlane(far);
}

float& Camera::GetFarPlane()
{
    return m_pCameraImpl->GetFarPlane();
}

float Camera::GetFarPlane() const
{
    return m_pCameraImpl->GetFarPlane();
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