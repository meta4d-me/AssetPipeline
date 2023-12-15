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

PIMPL_SIMPLE_TYPE_APIS(Camera, ID);
PIMPL_SIMPLE_TYPE_APIS(Camera, Aspect);
PIMPL_SIMPLE_TYPE_APIS(Camera, Fov);
PIMPL_SIMPLE_TYPE_APIS(Camera, NearPlane);
PIMPL_SIMPLE_TYPE_APIS(Camera, FarPlane);
PIMPL_COMPLEX_TYPE_APIS(Camera, Eye);
PIMPL_COMPLEX_TYPE_APIS(Camera, LookAt);
PIMPL_COMPLEX_TYPE_APIS(Camera, Up);
PIMPL_STRING_TYPE_APIS(Camera, Name);

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