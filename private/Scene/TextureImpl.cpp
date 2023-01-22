#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
	: m_textureFormat(TextureFormat::Count)
	, m_rawTexture()
{
	Init(textureID, textureType, MoveTemp(texturePath));
}

void TextureImpl::ClearRawTexture()
{
	m_textureFormat = TextureFormat::Count;
	m_rawTexture.clear();
}

void TextureImpl::Init(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
{
	m_id = textureID;
	m_textureType = textureType;
	m_path = MoveTemp(texturePath);
}

}