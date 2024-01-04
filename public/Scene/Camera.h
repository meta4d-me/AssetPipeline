#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class CameraImpl;

class CORE_API Camera final
{
public:
	DECLARE_SCENE_CLASS(Camera);
	explicit Camera(CameraID id, const char* pName);
	
	EXPORT_SIMPLE_TYPE_APIS(Camera, ID);
	EXPORT_SIMPLE_TYPE_APIS(Camera, Aspect);
	EXPORT_SIMPLE_TYPE_APIS(Camera, Fov);
	EXPORT_SIMPLE_TYPE_APIS(Camera, NearPlane);
	EXPORT_SIMPLE_TYPE_APIS(Camera, FarPlane);
	EXPORT_STRING_TYPE_APIS(Camera, Name);
	EXPORT_COMPLEX_TYPE_APIS(Camera, Eye);
	EXPORT_COMPLEX_TYPE_APIS(Camera, LookAt);
	EXPORT_COMPLEX_TYPE_APIS(Camera, Up);
};

}