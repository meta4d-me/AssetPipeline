#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/VectorDerived.hpp"
#include "Scene/ObjectID.h"

namespace cd
{

class LightImpl;

class CORE_API Light final
{
public:
	Light() = delete;
	explicit Light(InputArchive& inputArchive);
	explicit Light(InputArchiveSwapBytes & inputArchive);
	explicit Light(LightID lightID, const float type);
	Light(const Light&) = delete;
	Light& operator=(const Light&) = delete;
	Light(Light&&);
	Light& operator=(Light&&);
	~Light();

	void Init(LightID lightID, const float type);
	const LightID& GetID() const;
	const float& GetType() const;

	void SetIntensity(const float intensity);
	float& GetIntensity();
	const float& GetIntensity() const;

	void SetRange(const float range);
	float& GetRange();
	const float& GetRange() const;

	void SetRadius(const float radius);
	float& GetRadius();
	const float& GetRadius() const;

	void SetWidth(const float width);
	float& GetWidth();
	const float& GetWidth() const;

	void SetHeight(const float height);
	float& GetHeight();
	const float& GetHeight() const;

	void SetAngleScale(const float angleScale);
	float& GetAngleScale();
	const float& GetAngleScale() const;

	void SetAngleOffeset(const float angleOffeset);
	float& GetAngleOffeset();
	const float& GetAngleOffeset() const;

	void SetPosition(cd::Point position);
	cd::Point& GetPosition();
	const cd::Point& GetPosition() const;

	void SetColor(cd::Vec3f color);
	cd::Vec3f& GetColor();
	const cd::Vec3f& GetColor() const;

	void SetDirection(cd::Direction direction);
	cd::Direction& GetDirection();
	const cd::Direction& GetDirection() const;

	void SetUp(cd::Direction up);
	cd::Direction& GetUp();
	const cd::Direction& GetUp() const;

	const std::pair<float, float> CalculateScaleAndOffeset(const float innerAngle, const float outerAngle) const;

	Light& operator<<(InputArchive& inputArchive);
	Light& operator<<(InputArchiveSwapBytes& inputArchive);
	const Light& operator>>(OutputArchive& outputArchive) const;
	const Light& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	LightImpl* m_pLightImpl = nullptr;
};

}