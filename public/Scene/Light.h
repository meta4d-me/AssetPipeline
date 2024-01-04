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
	DECLARE_SCENE_CLASS(Light);
	explicit Light(LightID lightID, LightType type);
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
	EXPORT_STRING_TYPE_APIS(Light, Name);
	EXPORT_COMPLEX_TYPE_APIS(Light, Color);
	EXPORT_COMPLEX_TYPE_APIS(Light, Position);
	EXPORT_COMPLEX_TYPE_APIS(Light, Direction);
	EXPORT_COMPLEX_TYPE_APIS(Light, Up);

	std::pair<float, float> CalculateScaleAndOffset(float innerAngle, float outerAngle) const;
};

}