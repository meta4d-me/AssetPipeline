#pragma once

#include <inttypes.h>

namespace cdtools
{

enum class MaterialTextureType : uint8_t
{
	BaseColor = 0,
	Normal,
	Metalness,
	Roughness,
	Emissive,
	AO,
	Count
};

constexpr const char* MaterialTextureTypeName[] =
{
	"BaseColor",
	"Normal",
	"Metalness",
	"Roughness",
	"Emissive",
	"AO",
};

// Sanity check for enum and name mapping.
static_assert(static_cast<int>(MaterialTextureType::Count) == sizeof(MaterialTextureTypeName) / sizeof(char*),
	"Material texture types and names mismatch.");

inline const char* GetMaterialTextureTypeName(MaterialTextureType textureType)
{
	return MaterialTextureTypeName[static_cast<int>(textureType)];
}

}