#include "MaterialImpl.h"

#include <cassert>

namespace cd
{

MaterialImpl::MaterialImpl(MaterialID materialID, std::string materialName)
{
	Init(materialID, MoveTemp(materialName));
	SetPropertyDefaultValue();
}

void MaterialImpl::Init(MaterialID materialID, std::string materialName)
{
	m_id = materialID;
	m_name = MoveTemp(materialName);
}

void MaterialImpl::SetPropertyDefaultValue()
{
	m_basePBRMaterialType.Add("Name", std::string("BasePBR"));

	m_basePBRMaterialType.Add("BaseColor_Color_R", 1.0f);
	m_basePBRMaterialType.Add("BaseColor_Color_G", 1.0f);
	m_basePBRMaterialType.Add("BaseColor_Color_B", 1.0f);
	m_basePBRMaterialType.Add("BaseColor_Factor", 0.0f);
	m_basePBRMaterialType.Add("BaseColor_UseTexture", true);

	m_basePBRMaterialType.Add("Occlusion_Factor", 0.0f);
	m_basePBRMaterialType.Add("Occlusion_UseTexture", true);

	m_basePBRMaterialType.Add("Roughness_Factor", 0.9f);
	m_basePBRMaterialType.Add("Roughness_UseTexture", true);

	m_basePBRMaterialType.Add("Metallic_Factor", 0.1f);
	m_basePBRMaterialType.Add("Metallic_UseTexture", true);

	m_basePBRMaterialType.Add("Normal_UseTexture", true);

	m_basePBRMaterialType.Add("General_EnableDirectionalLights", true);
	m_basePBRMaterialType.Add("General_EnablePunctualLights", true);
	m_basePBRMaterialType.Add("General_EnableAreaLights", false);
	m_basePBRMaterialType.Add("General_EnableIBL", true);
}

void MaterialImpl::SetTextureID(MaterialPropretyGroup propretyGroup, TextureID textureID)
{
	m_basePBRMaterialType.Add(GetMaterialPropretyTextureKey(propretyGroup), textureID.Data());
}

std::optional<TextureID> MaterialImpl::GetTextureID(MaterialPropretyGroup propretyGroup) const
{
	static_assert(sizeof(TextureID) == sizeof(uint32_t));
	const auto textureID = m_basePBRMaterialType.Get<uint32_t>(GetMaterialPropretyTextureKey(propretyGroup));
	if (textureID.has_value())
	{
		return TextureID(textureID.value());
	}
	else
	{
		return std::nullopt;
	}
}

bool MaterialImpl::IsTextureTypeSetup(MaterialPropretyGroup propretyGroup) const
{
	return m_basePBRMaterialType.Exist(GetMaterialPropretyTextureKey(propretyGroup));
}

}