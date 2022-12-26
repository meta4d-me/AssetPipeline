#include "MaterialImpl.h"

#include <cassert>

namespace cd
{

MaterialImpl::MaterialImpl(MaterialID materialID, std::string materialName)
{
	Init(materialID, MoveTemp(materialName));
}

void MaterialImpl::Init(MaterialID materialID, std::string materialName)
{
	m_id = materialID;
	m_name = MoveTemp(materialName);
}

void MaterialImpl::SetTextureID(MaterialTextureType textureType, TextureID textureID)
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

std::optional<TextureID> MaterialImpl::GetTextureID(MaterialTextureType textureType) const
{
	TextureIDMap::const_iterator itTexture = m_textureIDs.find(textureType);
	if(itTexture != m_textureIDs.end())
	{
		return itTexture->second;
	}

	return std::nullopt;
}

}