#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

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

	void SetID(uint32_t id) { m_id = CameraID(id); }
	const CameraID& GetID() const { return m_id; }

	void SetName(std::string name) { m_name = cd::MoveTemp(name); }
	std::string& GetName() { return m_name; }
	const std::string& GetName() const { return m_name; }

	void SetPosition(Vec3f position) { m_position = MoveTemp(position); }
	Vec3f& GetPosition() { return m_position; }
	const Vec3f& GetPosition() const { return m_position; }

	void SetLookAt(Vec3f lookAt) { m_lookAt = MoveTemp(lookAt); }
	Vec3f& GetLookAt() { return m_lookAt; }
	const Vec3f& GetLookAt() const { return m_lookAt; }

	void SetUp(Vec3f up) { m_up = MoveTemp(up); }
	Vec3f& GetUp() { return m_up; }
	const Vec3f& GetUp() const { return m_up; }

	void SetAspect(float aspect) { m_aspect = aspect; }
	float GetAspect() const { return m_aspect; }

	void SetFov(float fov) { m_fov = fov; }
	float GetFov() const { return m_fov; }

	void SetNearPlane(float near) { m_near = near; }
	float GetNearPlane() const { return m_near; }

	void SetFarPlane(float far) { m_far = far; }
	float GetFarPlane() const { return m_far; }

	template<bool SwapBytesOrder>
	CameraImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t cameraID;
		std::string cameraName;
		Vec3f position;
		Vec3f lookAt;
		Vec3f up;
		float aspect;
		float fov;
		float near;
		float far;

		inputArchive >> cameraID >> cameraName
			>> position >> lookAt >> up
			>> aspect >> fov >> near >> far;

		Init(CameraID(cameraID), cd::MoveTemp(cameraName));
		SetPosition(MoveTemp(position));
		SetLookAt(MoveTemp(lookAt));
		SetUp(MoveTemp(up));
		SetAspect(aspect);
		SetFov(fov);
		SetNearPlane(near);
		SetFarPlane(far);

		return *this;
	}

	template<bool SwapBytesOrder>
	const CameraImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName()
			<< GetPosition() << GetLookAt() << GetUp()
			<< GetAspect() << GetFov() << GetNearPlane() << GetFarPlane();

		return *this;
	}

private:
	CameraID m_id;
	Vec3f m_position;
	Vec3f m_lookAt;
	Vec3f m_up;
	float m_aspect;
	float m_fov;
	float m_near;
	float m_far;
	std::string m_name;
};

}