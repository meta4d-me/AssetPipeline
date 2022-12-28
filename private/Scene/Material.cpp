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

Material::Material(MaterialID materialID, const char* pMaterialName)
{
	m_pMaterialImpl = new MaterialImpl(materialID, pMaterialName);
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

void Material::Init(MaterialID materialID, const char* pMaterialName)
{
	m_pMaterialImpl->Init(materialID, pMaterialName);
}

const MaterialID& Material::GetID() const
{
	return m_pMaterialImpl->GetID();
}

const char* Material::GetName() const
{
	return m_pMaterialImpl->GetName().c_str();
}

void Material::SetTextureID(MaterialTextureType textureType, TextureID textureID)
{
	m_pMaterialImpl->SetTextureID(textureType, textureID);
}

std::optional<TextureID> Material::GetTextureID(MaterialTextureType textureType) const
{
	return m_pMaterialImpl->GetTextureID(textureType);
}

const Material::TextureIDMap& Material::GetTextureIDMap() const
{
	return m_pMaterialImpl->GetTextureIDMap();
}

bool Material::IsTextureTypeSetup(MaterialTextureType textureType) const
{
	return m_pMaterialImpl->IsTextureTypeSetup(textureType);
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