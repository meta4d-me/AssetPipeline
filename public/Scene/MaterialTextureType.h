#pragma once

#include <cstdint>
#include <sstream>

namespace cd
{

DEFINE_ENUM_WITH_NAMES(MaterialType, BasePBR);
DEFINE_ENUM_WITH_NAMES(MaterialPropertyGroup, BaseColor, Occlusion, Roughness, Metallic, Normal, Emissive, Elevation, AlphaMap, General);
DEFINE_ENUM_WITH_NAMES(MaterialProperty, Name, Factor, Texture, UseTexture, Color, EnableDirectionalLights, EnablePunctualLights, EnableAreaLights, EnableIBL);

// As each MaterialPropertyGroup just has one texture,
// it's fine to consider MaterialPropertyGroup as MaterialTextureType
// from the perspective of texture.
using MaterialTextureType = MaterialPropertyGroup;

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

}