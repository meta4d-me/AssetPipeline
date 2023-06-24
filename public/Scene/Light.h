#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Vector.hpp"
#include "Scene/LightType.h"
#include "Scene/ObjectID.h"

namespace cd
{

class LightImpl;

class CORE_API Light final
{
public:
	static const char* GetClassName() { return "Light"; }

public:
	Light() = delete;
	explicit Light(InputArchive& inputArchive);
	explicit Light(InputArchiveSwapBytes & inputArchive);
	explicit Light(LightID lightID, LightType type);
	Light(const Light&) = delete;
	Light& operator=(const Light&) = delete;
	Light(Light&&);
	Light& operator=(Light&&);
	~Light();

	void Init(LightID lightID, LightType type);

	LightID GetID() const;
	LightType GetType() const;

	void SetName(const char* pName);
	const char* GetName() const;

	void SetIntensity(float intensity);
	float& GetIntensity();
	float GetIntensity() const;

	void SetRange(float range);
	float& GetRange();
	float GetRange() const;

	void SetRadius(float radius);
	float& GetRadius();
	float GetRadius() const;

	void SetWidth(float width);
	float& GetWidth();
	float GetWidth() const;

	void SetHeight(float height);
	float& GetHeight();
	float GetHeight() const;

	void SetAngleScale(float angleScale);
	float& GetAngleScale();
	float GetAngleScale() const;

	void SetAngleOffset(float angleOffset);
	float& GetAngleOffset();
	float GetAngleOffset() const;

	void SetColor(cd::Vec3f color);
	cd::Vec3f& GetColor();
	const cd::Vec3f& GetColor() const;

	void SetPosition(cd::Point position);
	cd::Point& GetPosition();
	const cd::Point& GetPosition() const;

	void SetDirection(cd::Direction direction);
	cd::Direction& GetDirection();
	const cd::Direction& GetDirection() const;

	void SetUp(cd::Direction up);
	cd::Direction& GetUp();
	const cd::Direction& GetUp() const;

	std::pair<float, float> CalculateScaleAndOffset(float innerAngle, float outerAngle) const;

	Light& operator<<(InputArchive& inputArchive);
	Light& operator<<(InputArchiveSwapBytes& inputArchive);
	const Light& operator>>(OutputArchive& outputArchive) const;
	const Light& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	LightImpl* m_pLightImpl = nullptr;
};

}