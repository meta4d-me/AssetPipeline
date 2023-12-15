#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Vector.hpp"
#include "Scene/LightType.h"
#include "Scene/Types.h"

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

	EXPORT_SIMPLE_TYPE_APIS(Light, ID);
	EXPORT_SIMPLE_TYPE_APIS(Light, Type);
	EXPORT_SIMPLE_TYPE_APIS(Light, Intensity);
	EXPORT_SIMPLE_TYPE_APIS(Light, Range);
	EXPORT_SIMPLE_TYPE_APIS(Light, Radius);
	EXPORT_SIMPLE_TYPE_APIS(Light, Width);
	EXPORT_SIMPLE_TYPE_APIS(Light, Height);
	EXPORT_SIMPLE_TYPE_APIS(Light, AngleScale);
	EXPORT_SIMPLE_TYPE_APIS(Light, AngleOffset);
	EXPORT_COMPLEX_TYPE_APIS(Light, Color);
	EXPORT_COMPLEX_TYPE_APIS(Light, Position);
	EXPORT_COMPLEX_TYPE_APIS(Light, Direction);
	EXPORT_COMPLEX_TYPE_APIS(Light, Up);
	EXPORT_STRING_TYPE_APIS(Light, Name);

	std::pair<float, float> CalculateScaleAndOffset(float innerAngle, float outerAngle) const;

	Light& operator<<(InputArchive& inputArchive);
	Light& operator<<(InputArchiveSwapBytes& inputArchive);
	const Light& operator>>(OutputArchive& outputArchive) const;
	const Light& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	LightImpl* m_pLightImpl = nullptr;
};

}