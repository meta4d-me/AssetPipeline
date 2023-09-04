#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class CameraImpl;

class CORE_API Camera final
{
public:
	static const char* GetClassName() { return "Camera"; }

public:
	Camera() = delete;
	explicit Camera(InputArchive& inputArchive);
	explicit Camera(InputArchiveSwapBytes& inputArchive);
	explicit Camera(CameraID id, const char* pName);
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&);
	Camera& operator=(Camera&&);
	~Camera();
	
	EXPORT_OBJECT_ID_APIS(CameraID);
	EXPORT_NAME_APIS();

	void SetEye(Vec3f eye);
	Vec3f& GetEye();
	const Vec3f& GetEye() const;

	void SetLookAt(Vec3f lookAt);
	Vec3f& GetLookAt();
	const Vec3f& GetLookAt() const;

	void SetUp(Vec3f up);
	Vec3f& GetUp();
	const Vec3f& GetUp() const;

	void SetAspect(float aspect);
	float& GetAspect();
	float GetAspect() const;

	void SetFov(float fov);
	float& GetFov();
	float GetFov() const;

	void SetNearPlane(float near);
	float& GetNearPlane();
	float GetNearPlane() const;

	void SetFarPlane(float far);
	float& GetFarPlane();
	float GetFarPlane() const;

	Camera& operator<<(InputArchive& inputArchive);
	Camera& operator<<(InputArchiveSwapBytes& inputArchive);
	const Camera& operator>>(OutputArchive& outputArchive) const;
	const Camera& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	CameraImpl* m_pCameraImpl = nullptr;
};

}