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
	Tube,

	Count,
};

constexpr const char *LightTypeName[] =
{
	"Point",
	"Spot",
	"Directional",
	"Sphere",
	"Disk",
	"Rectangle",
	"Tube",
};

CD_FORCEINLINE const char *GetLightTypeName(LightType lightType) {
	return LightTypeName[static_cast<size_t>(lightType)];
}

static_assert(static_cast<int>(LightType::Count) == sizeof(LightTypeName) / sizeof(char *), "Light type and names mismatch.");

} // namespace cd
