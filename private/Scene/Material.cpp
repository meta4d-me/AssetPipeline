#include "Scene/Material.h"
#include "MaterialImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Material);

Material::Material(MaterialID materialID, const char* pMaterialName, MaterialType type)
{
	m_pMaterialImpl = new MaterialImpl(materialID, pMaterialName, type);
}

void Material::Init(MaterialID materialID, const char* pMaterialName, MaterialType type)
{
	m_pMaterialImpl->Init(materialID, pMaterialName, type);
}

PIMPL_SIMPLE_TYPE_APIS(Material, ID);
PIMPL_SIMPLE_TYPE_APIS(Material, Type);
PIMPL_STRING_TYPE_APIS(Material, Name);

const PropertyMap& Material::GetPropertyGroups() const
{
	return m_pMaterialImpl->GetPropertyGroups();
}

void Material::SetTextureID(MaterialTextureType textureType, TextureID textureID)
{
	m_pMaterialImpl->SetProperty(textureType, MaterialProperty::Texture, textureID.Data());
}

TextureID Material::GetTextureID(MaterialTextureType textureType) const
{
	auto textureID = m_pMaterialImpl->GetProperty<uint32_t>(textureType, MaterialProperty::Texture);
	return textureID.has_value() ? TextureID(textureID.value()) : TextureID(cd::TextureID::InvalidID);
}

void Material::RemoveTexture(MaterialTextureType textureType)
{
	m_pMaterialImpl->RemoveProperty(textureType, MaterialProperty::Texture);
}

bool Material::IsTextureSetup(MaterialTextureType textureType) const
{
	return m_pMaterialImpl->ExistProperty(textureType, MaterialProperty::Texture);
}

void Material::RemoveProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property)
{
	m_pMaterialImpl->RemoveProperty(propertyGroup, property);
}

void Material::SetBoolProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, bool value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, static_cast<int32_t>(value));
}

void Material::SetI32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, int32_t value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetI64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, int64_t value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetU32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, uint32_t value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetU64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, uint64_t value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetFloatProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, float value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetDoubleProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, double value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetStringProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const std::string& value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetVec2fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const cd::Vec2f& value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

void Material::SetVec3fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const cd::Vec3f& value)
{
	m_pMaterialImpl->SetProperty(propertyGroup, property, value);
}

std::optional<bool> Material::GetBoolProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	auto optBool = m_pMaterialImpl->GetProperty<int32_t>(propertyGroup, property);
	if (optBool.has_value())
	{
		return optBool.value() == 1;
	}

	return std::nullopt;
}

std::optional<int32_t> Material::GetI32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<int32_t>(propertyGroup, property);
}

std::optional<int64_t> Material::GetI64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<int64_t>(propertyGroup, property);
}

std::optional<uint32_t> Material::GetU32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<uint32_t>(propertyGroup, property);
}

std::optional<uint64_t> Material::GetU64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<uint64_t>(propertyGroup, property);
}

std::optional<float> Material::GetFloatProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<float>(propertyGroup, property);
}

std::optional<double> Material::GetDoubleProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<double>(propertyGroup, property);
}

std::optional<std::string> Material::GetStringProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<std::string>(propertyGroup, property);
}

std::optional<cd::Vec2f> Material::GetVec2fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->GetProperty<cd::Vec2f>(propertyGroup, property);
}

bool Material::ExistProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
{
	return m_pMaterialImpl->ExistProperty(propertyGroup, property);
}

}