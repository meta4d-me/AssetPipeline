#pragma once

#include "Base/NameOf.h"

#include <cstdint>
#include <sstream>

namespace cd
{

enum class MaterialType : uint8_t
{
	BasePBR = 0
};

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
	General
};

// As each MaterialPropertyGroup just has one texture,
// it's fine to consider MaterialPropertyGroup as MaterialTextureType
// from the perspective of texture.
using MaterialTextureType = MaterialPropertyGroup;

enum class MaterialProperty
{
	Name = 0,

	// For BaseColor, Occlusion, Roughness, Metallic and Normal
	Factor,
	Texture,
	UseTexture,
	UVOffset,
	UVScale,

	// Just for BaseColor
	Color,

	// Just for General Settings
	BlendMode,
	OpacityMaskClipValue, // BlendMode_Mask
	TwoSided,
	EnableDirectionalLights,
	EnablePunctualLights,
	EnableAreaLights,
	EnableIBL
};

CD_FORCEINLINE std::string GetMaterialPropertyKey(MaterialPropertyGroup propertyGroup, MaterialProperty property)
{
	std::stringstream ss;
	ss << nameof::nameof_enum(propertyGroup) << "_" << nameof::nameof_enum(property);

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
	Border
};

enum class BlendMode
{
	Opaque,
	Mask,
	Blend
};

}