#pragma once

#include <cstdint>
#include <sstream>

namespace cd
{

enum class MaterialType : uint8_t
{
	BasePBR = 0,
	
	Count,
};

constexpr const char *MaterialTypeName[] =
{
	"BasePBR",
};

static_assert(static_cast<int>(MaterialType::Count) == sizeof(MaterialTypeName) / sizeof(char *),
	"Material type and names mismatch.");

CD_FORCEINLINE const char *GetMaterialTypeName(MaterialType materialType)
{
	return MaterialTypeName[static_cast<size_t>(materialType)];
}

enum class MaterialPropertyGroup
{
	BaseColor = 0,
	Occlusion,
	Roughness,
	Metallic,
	Normal,
	Emissive,
	Elevation,
	AlphaMap,
	General,

	Count,
};

// As each MaterialPropertyGroup just has one texture,
// it's fine to consider MaterialPropertyGroup as MaterialTextureType
// from the perspective of texture.
using MaterialTextureType = MaterialPropertyGroup;

constexpr const char *MaterialPropertyGroupName[] =
{
	"BaseColor",
	"Occlusion",
	"Roughness",
	"Metallic",
	"Normal",
	"Emissive",
	"Elevation",
	"AlphaMap",
	"General",
};

static_assert(static_cast<int>(MaterialPropertyGroup::Count) == sizeof(MaterialPropertyGroupName) / sizeof(char *),
	"Material property group and names mismatch.");

CD_FORCEINLINE const char *GetMaterialPropertyGroupName(MaterialPropertyGroup propertyGroup) {
	return MaterialPropertyGroupName[static_cast<size_t>(propertyGroup)];
}

enum class MaterialProperty
{
	Name = 0,

	// For BaseColor, Occlusion, Roughness, Metallic and Normal
	Factor,
	Texture,
	UseTexture,

	// Just for BaseColor
	Color,

	// Just for General Settings
	EnableDirectionalLights,
	EnablePunctualLights,
	EnableAreaLights,
	EnableIBL,

	Count,
};

constexpr const char *MaterialPropertyName[] =
{
	"Name",
	"Factor",
	"Texture",
	"UseTexture",
	"Color",
	"EnableDirectionalLights",
	"EnablePunctualLights",
	"EnableAreaLights",
	"EnableIBL",
};

static_assert(static_cast<int>(MaterialProperty::Count) == sizeof(MaterialPropertyName) / sizeof(char *),
	"Material property and names mismatch.");

CD_FORCEINLINE const char *GetMaterialPropertyName(MaterialProperty property) {
	return MaterialPropertyName[static_cast<size_t>(property)];
}

CD_FORCEINLINE std::string GetMaterialPropertyKey(MaterialPropertyGroup propertyGroup, MaterialProperty property)
{
	std::stringstream ss;
	ss << GetMaterialPropertyGroupName(propertyGroup) << "_" << GetMaterialPropertyName(property);

	return ss.str();
}

CD_FORCEINLINE std::string GetMaterialPropertyTextureKey(MaterialPropertyGroup propertyGroup)
{
	return GetMaterialPropertyKey(propertyGroup, MaterialProperty::Texture);
}

enum class TextureMapMode
{
	Wrap,
	Clamp,
	Mirror,
	Border,
	Count
};

constexpr const char* TextureMapModeNames[] =
{
	"Wrap",
	"Clamp",
	"Mirror",
	"Border",
};

static_assert(static_cast<int>(TextureMapMode::Count) == sizeof(TextureMapModeNames) / sizeof(char*),
	"Material texture map modes and names mismatch.");

static constexpr const char* GetTextureMapModeName(cd::TextureMapMode mapMode)
{
	return TextureMapModeNames[static_cast<uint8_t>(mapMode)];
}

}