#pragma once

#include "Base/Platform.h"

namespace cd
{

// Note that this enum should align with U_Light.sh definitions.
enum class LightType : uint8_t
{
	Point = 0,
	Spot,
	Directional,

	// Areal light
	Sphere,
	Disk,
	Rectangle,
	Tube
};

}
