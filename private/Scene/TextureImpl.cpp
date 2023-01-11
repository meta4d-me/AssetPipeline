#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
{
	Init(textureID, textureType, MoveTemp(texturePath));
}

void TextureImpl::Init(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
{
	m_id = textureID;
	m_textureType = textureType;
	m_path = MoveTemp(texturePath);
}

}