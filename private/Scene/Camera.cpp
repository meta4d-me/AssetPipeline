#include "Scene/Camera.h"
#include "CameraImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Camera);

Camera::Camera(CameraID id, const char* pName)
{
    m_pCameraImpl = new CameraImpl(id, pName);
}

PIMPL_SIMPLE_TYPE_APIS(Camera, ID);
PIMPL_SIMPLE_TYPE_APIS(Camera, Aspect);
PIMPL_SIMPLE_TYPE_APIS(Camera, Fov);
PIMPL_SIMPLE_TYPE_APIS(Camera, NearPlane);
PIMPL_SIMPLE_TYPE_APIS(Camera, FarPlane);
PIMPL_STRING_TYPE_APIS(Camera, Name);
PIMPL_COMPLEX_TYPE_APIS(Camera, Eye);
PIMPL_COMPLEX_TYPE_APIS(Camera, LookAt);
PIMPL_COMPLEX_TYPE_APIS(Camera, Up);

}