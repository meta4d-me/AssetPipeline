#include "Material.h"

#include <cassert>

namespace cd
{

Material::Material(MaterialID materialID, std::string materialName)
{
	Init(materialID, MoveTemp(materialName));
}

void Material::Init(MaterialID materialID, std::string materialName)
{
	m_id = materialID;
	m_name = MoveTemp(materialName);
}

void Material::SetTextureID(MaterialTextureType textureType, TextureID textureID)
{
	TextureIDMap::iterator itTexture = m_textureIDs.find(textureType);
	if(itTexture != m_textureIDs.end())
	{
		// Existed!
		if(textureID == itTexture->second)
		{
			// Same
			return;
		}
	}

	m_textureIDs[textureType] = textureID;
}

std::optional<TextureID> Material::GetTextureID(MaterialTextureType textureType) const
{
	TextureIDMap::const_iterator itTexture = m_textureIDs.find(textureType);
	if(itTexture != m_textureIDs.end())
	{
		return itTexture->second;
	}

	return std::nullopt;
}

}