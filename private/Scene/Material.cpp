#include "Scene/Material.h"
#include "MaterialImpl.h"

namespace cd
{

Material::Material(InputArchive& inputArchive)
{
	m_pMaterialImpl = new MaterialImpl(inputArchive);
}

Material::Material(InputArchiveSwapBytes& inputArchive)
{
	m_pMaterialImpl = new MaterialImpl(inputArchive);
}

Material::Material(MaterialID materialID, const char* pMaterialName, MaterialType type)
{
	m_pMaterialImpl = new MaterialImpl(materialID, pMaterialName, type);
}

Material::Material(Material&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

Material& Material::operator=(Material&& rhs)
{
	std::swap(m_pMaterialImpl, rhs.m_pMaterialImpl);
	return *this;
}

Material::~Material()
{
	if (m_pMaterialImpl)
	{
		delete m_pMaterialImpl;
		m_pMaterialImpl = nullptr;
	}
}

void Material::Init(MaterialID materialID, const char* pMaterialName, MaterialType type)
{
	m_pMaterialImpl->Init(materialID, pMaterialName, type);
}

const MaterialID& Material::GetID() const
{
	return m_pMaterialImpl->GetID();
}

const char* Material::GetName() const
{
	return m_pMaterialImpl->GetName().c_str();
}

void Material::AddTextureID(MaterialTextureType textureType, TextureID textureID)
{
	m_pMaterialImpl->AddTextureID(textureType, textureID);
}

std::optional<TextureID> Material::GetTextureID(MaterialTextureType textureType) const
{
	return m_pMaterialImpl->GetTextureID(textureType);
}

const PropertyMap& Material::GetPropertyGroups() const {
	return m_pMaterialImpl->GetPropertyGroups();
}

bool Material::IsTextureSetup(MaterialTextureType textureType) const
{
	return m_pMaterialImpl->IsTextureSetup(textureType);
}

Material& Material::operator<<(InputArchive& inputArchive)
{
	*m_pMaterialImpl << inputArchive;
	return *this;
}

Material& Material::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pMaterialImpl << inputArchive;
	return *this;
}

const Material& Material::operator>>(OutputArchive& outputArchive) const
{
	*m_pMaterialImpl >> outputArchive;
	return *this;
}

const Material& Material::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pMaterialImpl >> outputArchive;
	return *this;
}

}