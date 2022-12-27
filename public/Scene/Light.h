#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/VectorDerived.hpp"
#include "ObjectID.h"

#include <string>

namespace cd
{

class LightImpl;

class TOOL_API Light final
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

	template<bool SwapBytesOrder>
	Light& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t lightID;
		float lightType, lightIntensity, lightRange, lightRadius,
			lightWidth, lightHeight, lightAngleScale, lightAngleOffeset;

		inputArchive >> lightID >> lightType >> lightIntensity >> lightRange >>
			lightRadius >> lightWidth >> lightHeight >> lightAngleScale >> lightAngleOffeset;

		Init(LightID(lightID), lightType);
		SetIntensity(lightIntensity);
		SetRange(lightRange);
		SetRadius(lightRadius);
		SetWidth(lightWidth);
		SetHeight(lightHeight);
		SetAngleScale(lightAngleScale);
		SetAngleOffeset(lightAngleOffeset);

		inputArchive.ImportBuffer(&GetPosition());
		inputArchive.ImportBuffer(&GetColor());
		inputArchive.ImportBuffer(&GetDirection());
		inputArchive.ImportBuffer(&GetUp());

		return *this;
	}

	template<bool SwapBytesOrder>
	const Light& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetType() << GetIntensity() << GetRange() <<
			GetRadius() << GetWidth() << GetHeight() << GetAngleScale() << GetAngleOffeset();

		outputArchive.ExportBuffer(&GetPosition(), 1);
		outputArchive.ExportBuffer(&GetColor(), 1);
		outputArchive.ExportBuffer(&GetDirection(), 1);
		outputArchive.ExportBuffer(&GetUp(), 1);

		return *this;
	}

private:
	LightImpl* m_pLightImpl = nullptr;
};

}