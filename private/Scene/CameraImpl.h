#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class CameraImpl final
{
public:
	CameraImpl() = delete;
	template<bool SwapBytesOrder>
	explicit CameraImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit CameraImpl(CameraID id, std::string name);
	CameraImpl(const CameraImpl&) = default;
	CameraImpl& operator=(const CameraImpl&) = default;
	CameraImpl(CameraImpl&&) = default;
	CameraImpl& operator=(CameraImpl&&) = default;
	~CameraImpl() = default;

	void Init(CameraID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(Camera, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Camera, Aspect);
	IMPLEMENT_SIMPLE_TYPE_APIS(Camera, Fov);
	IMPLEMENT_SIMPLE_TYPE_APIS(Camera, NearPlane);
	IMPLEMENT_SIMPLE_TYPE_APIS(Camera, FarPlane);
	IMPLEMENT_COMPLEX_TYPE_APIS(Camera, Eye);
	IMPLEMENT_COMPLEX_TYPE_APIS(Camera, LookAt);
	IMPLEMENT_COMPLEX_TYPE_APIS(Camera, Up);
	IMPLEMENT_STRING_TYPE_APIS(Camera, Name);

	template<bool SwapBytesOrder>
	CameraImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t cameraID;
		std::string cameraName;
		inputArchive >> cameraID >> cameraName;
		Init(CameraID(cameraID), cd::MoveTemp(cameraName));
		inputArchive >> GetEye() >> GetLookAt() >> GetUp()
			>> GetNearPlane() >> GetFarPlane() >> GetAspect() >> GetFov();
		
		return *this;
	}

	template<bool SwapBytesOrder>
	const CameraImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName()
			<< GetEye() << GetLookAt() << GetUp()
			<< GetNearPlane() << GetFarPlane() << GetAspect() << GetFov();

		return *this;
	}
};

}