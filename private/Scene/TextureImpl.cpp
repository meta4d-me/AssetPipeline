#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
	: m_format(TextureFormat::Count)
	, m_rawData()
	, m_width(0)
	, m_height(0)
{
	Init(textureID, textureType, MoveTemp(texturePath));
}

void TextureImpl::ClearRawTexture()
{
	m_format = TextureFormat::Count;
	m_rawData.clear();
	m_width = 0;
	m_height = 0;
}

void TextureImpl::Init(TextureID textureID, MaterialTextureType textureType, std::string texturePath)
{
	m_id = textureID;
	m_type = textureType;
	m_uvMapMode[0] = TextureMapMode::Wrap;
	m_uvMapMode[1] = TextureMapMode::Wrap;
	m_path = MoveTemp(texturePath);
}

}