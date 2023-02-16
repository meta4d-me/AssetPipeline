#pragma once

#include <cstdint>
#include <sstream>

namespace cd
{

enum class MaterialPropretyGroup
{
	BaseColor = 0,
	Occlusion,
	Roughness,
	Metallic,
	Normal,
	Emissive,
	General,
	Count,
};

using MaterialTextureType = MaterialPropretyGroup;

constexpr const char *MaterialPropretyGroupName[] =
{
	"BaseColor",
	"Occlusion",
	"Roughness",
	"Metallic",
	"Normal",
	"Emissive",
	"General",
};

static_assert(static_cast<int>(MaterialPropretyGroup::Count) == sizeof(MaterialPropretyGroupName) / sizeof(char *),
	"Material proprety group and names mismatch.");

inline const char *GetMaterialPropretyGroupName(MaterialPropretyGroup propretyGroup) {
	return MaterialPropretyGroupName[static_cast<size_t>(propretyGroup)];
}

enum class MaterialProperty
{
	Name = 0,
	Factor,
	Texture,
	UseTexture,

	// Just for BaseColor
	Color_R,
	Color_G,
	Color_B,

	// Just for General Settings
	EnableDirectionalLights,
	EnablePunctualLights,
	EnableAreaLights,
	EnableIBL,

	Count,
};

constexpr const char *MaterialPropretyName[] =
{
	"Name",
	"Factor",
	"Texture",
	"UseTexture",
	"Color_R",
	"Color_G",
	"Color_B",
	"EnableDirectionalLights",
	"EnablePunctualLights",
	"EnableAreaLights",
	"EnableIBL",
};

static_assert(static_cast<int>(MaterialProperty::Count) == sizeof(MaterialPropretyName) / sizeof(char *),
	"Material proprety and names mismatch.");

inline const char *GetMaterialPropretyName(MaterialProperty proprety) {
	return MaterialPropretyName[static_cast<size_t>(proprety)];
}

inline std::string GetMaterialPropretyKey(MaterialPropretyGroup propretyGroup, MaterialProperty property)
{
	std::stringstream ss;
	ss << GetMaterialPropretyGroupName(propretyGroup) << "_" << GetMaterialPropretyName(property);

	return ss.str();
}

inline std::string GetMaterialPropretyTextureKey(MaterialPropretyGroup propretyGroup)
{
	return GetMaterialPropretyKey(propretyGroup, MaterialProperty::Texture);
}

}