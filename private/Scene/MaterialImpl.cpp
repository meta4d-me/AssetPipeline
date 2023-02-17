#include "MaterialImpl.h"

#include <cassert>

namespace cd
{

MaterialImpl::MaterialImpl(MaterialID materialID, std::string materialName, MaterialType materialType)
{
	Init(materialID, MoveTemp(materialName), materialType);

	switch (materialType)
	{
		case MaterialType::BasePBR:
			InitBasePBR();
			break;

		default:
			printf("Unknow material type!\n");
			break;
	}
}

void MaterialImpl::Init(MaterialID materialID, std::string materialName, MaterialType materialType)
{
	m_id = materialID;
	m_name = MoveTemp(materialName);
	m_type = materialType;
}

void MaterialImpl::InitBasePBR()
{
	PropertyMap basePBR;

	basePBR.Add("BaseColor_Color", Vec3f(1.0f, 1.0f, 1.0f));
	// Factor for blending Color and Texture.
	// Or if there is no Texture, then this simply scales the Color value.
	basePBR.Add("BaseColor_Factor", 0.0f);
	basePBR.Add("BaseColor_UseTexture", true);

	basePBR.Add("Occlusion_Factor", 0.0f);
	basePBR.Add("Occlusion_UseTexture", true);

	basePBR.Add("Roughness_Factor", 0.9f);
	basePBR.Add("Roughness_UseTexture", true);

	basePBR.Add("Metallic_Factor", 0.1f);
	basePBR.Add("Metallic_UseTexture", true);

	basePBR.Add("Normal_UseTexture", true);

	basePBR.Add("General_EnableDirectionalLights", true);
	basePBR.Add("General_EnablePunctualLights", true);
	basePBR.Add("General_EnableAreaLights", false);
	basePBR.Add("General_EnableIBL", true);

	m_propertyGroups = std::move(basePBR);
}

void MaterialImpl::AddTextureID(MaterialPropertyGroup propertyGroup, TextureID textureID)
{
	static_assert(sizeof(TextureID) == sizeof(uint32_t));
	AddProperty(propertyGroup, MaterialProperty::Texture, textureID.Data());
}

std::optional<TextureID> MaterialImpl::GetTextureID(MaterialPropertyGroup propertyGroup) const
{
	static_assert(sizeof(TextureID) == sizeof(uint32_t));
	auto textureID = GetProperty<uint32_t>(propertyGroup, MaterialProperty::Texture);
	if (textureID.has_value())
	{
		return TextureID(textureID.value());
	}
	else
	{
		return std::nullopt;
	}
}

bool MaterialImpl::IsTextureSetup(MaterialPropertyGroup propertyGroup) const
{
	return ExtstProperty(propertyGroup, MaterialProperty::Texture);
}

}