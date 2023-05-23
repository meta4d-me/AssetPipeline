#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
	: m_textureFormat(TextureFormat::Count)
	, m_rawTexture()
	, m_textureWidth(0)
	, m_textureHeight(0)
{
	Init(textureID, textureType, MoveTemp(texturePath));
}

void TextureImpl::ClearRawTexture()
{
	m_textureFormat = TextureFormat::Count;
	m_rawTexture.clear();
	m_textureWidth = 0;
	m_textureHeight = 0;
}

void TextureImpl::Init(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
{
	m_id = textureID;
	m_textureType = textureType;
	m_textureUMapMode = TextureMapMode::Wrap;
	m_textureVMapMode = TextureMapMode::Wrap;
	m_path = MoveTemp(texturePath);
}

}