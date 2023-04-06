#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Vector.hpp"
#include "Scene/LightType.h"
#include "Scene/ObjectID.h"

#include <string>

namespace cd
{

class LightImpl final
{
public:
	LightImpl() = delete;
	explicit LightImpl(InputArchive& inputArchive);
	explicit LightImpl(InputArchiveSwapBytes & inputArchive);
	explicit LightImpl(LightID lightID, LightType type);
	LightImpl(const LightImpl&) = default;
	LightImpl& operator=(const LightImpl&) = default;
	LightImpl(LightImpl&&) = default;
	LightImpl& operator=(LightImpl&&) = default;
	~LightImpl() = default;

	void Init(LightID lightID, LightType type);

	LightID GetID() const { return m_id; }
	LightType GetType() const { return m_type; }

	void SetName(std::string name) { m_name = cd::MoveTemp(name); }
	std::string& GetName() { return m_name; }
	const std::string& GetName() const { return m_name; }

	void SetIntensity(float intensity) { m_intensity = intensity; }
	float& GetIntensity() { return m_intensity; }
	float GetIntensity() const { return m_intensity; }

	void SetRange(float range) { m_range = range; }
	float& GetRange() { return m_range; }
	float GetRange() const { return m_range; }

	void SetRadius(float radius) { m_radius = radius; }
	float& GetRadius() { return m_radius; }
	float GetRadius() const { return m_radius; }

	void SetWidth(float width) { m_width = width; }
	float& GetWidth() { return m_width; }
	float GetWidth() const { return m_width; }

	void SetHeight(float height) { m_height = height; }
	float& GetHeight() { return m_height; }
	float GetHeight() const { return m_height; }

	void SetAngleScale(float angleScale) { m_angleScale = angleScale; }
	float& GetAngleScale() { return m_angleScale; }
	float GetAngleScale() const { return m_angleScale; }

	void SetAngleOffset(float angleOffset) { m_angleOffset = angleOffset; }
	float& GetAngleOffset() { return m_angleOffset; }
	float GetAngleOffset() const { return m_angleOffset; }

	void SetColor(cd::Vec3f color) { m_color = cd::MoveTemp(color); }
	cd::Vec3f& GetColor() { return m_color; }
	const cd::Vec3f& GetColor() const { return m_color; }

	void SetPosition(cd::Point position) { m_position = cd::MoveTemp(position); }
	cd::Point& GetPosition() { return m_position; }
	const cd::Point& GetPosition() const { return m_position; }

	void SetDirection(cd::Direction direction) { m_direction = cd::MoveTemp(direction); }
	cd::Direction& GetDirection() { return m_direction; }
	const cd::Direction& GetDirection() const { return m_direction; }

	void SetUp(cd::Direction up) { m_up = cd::MoveTemp(up); }
	cd::Direction& GetUp() { return m_up; }
	const cd::Direction& GetUp() const { return m_up; }

	std::pair<float, float> CalculateScaleAndOffset(float innerAngle, float outerAngle) const;

	template<bool SwapBytesOrder>
	LightImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t lightID;
		uint8_t lightType;
		inputArchive >> lightID >> lightType;
		Init(LightID(lightID), static_cast<LightType>(lightType));
		inputArchive >> GetName() >> GetIntensity() >> GetRange() >> GetRadius()
			>> GetWidth() >> GetHeight() >> GetAngleScale() >> GetAngleOffset()
			>> GetColor() >> GetPosition() >> GetDirection() >> GetUp();
		return *this;
	}

	template<bool SwapBytesOrder>
	const LightImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << static_cast<uint8_t>(GetType())
			<< GetName() << GetIntensity() << GetRange() << GetRadius()
			<< GetWidth() << GetHeight() << GetAngleScale() << GetAngleOffset()
			<< GetColor() << GetPosition() << GetDirection() << GetUp();

		return *this;
	}

private:
	LightID m_id;
	LightType m_type;
	std::string m_name;

	float m_intensity;
	float m_range;
	float m_radius;
	float m_width;
	float m_height;
	float m_angleScale;
	float m_angleOffset;

	cd::Point m_position;
	cd::Vec3f m_color;
	cd::Direction m_direction;
	cd::Direction m_up;
};

} // namespace cd
