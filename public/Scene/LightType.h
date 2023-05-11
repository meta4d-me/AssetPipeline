#pragma once

#include "Base/Platform.h"

namespace cd
{

enum class LightType : uint8_t
{
	Point = 0,
	Directional,
	Spot,

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
	"Directional",
	"Spot",
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
