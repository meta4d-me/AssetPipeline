#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Vector.hpp"
#include "Scene/LightType.h"
#include "Scene/Types.h"

#include <string>

namespace cd
{

class LightImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Light);

	explicit LightImpl(LightID lightID, LightType type);
	void Init(LightID lightID, LightType type);

	IMPLEMENT_SIMPLE_TYPE_APIS(Light, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Type);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Intensity);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Range);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Radius);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Width);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, Height);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, AngleScale);
	IMPLEMENT_SIMPLE_TYPE_APIS(Light, AngleOffset);
	IMPLEMENT_COMPLEX_TYPE_APIS(Light, Color);
	IMPLEMENT_COMPLEX_TYPE_APIS(Light, Position);
	IMPLEMENT_COMPLEX_TYPE_APIS(Light, Direction);
	IMPLEMENT_COMPLEX_TYPE_APIS(Light, Up);
	IMPLEMENT_STRING_TYPE_APIS(Light, Name);

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
};

}