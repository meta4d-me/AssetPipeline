#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, const char* pName, MaterialTextureType textureType)
{
	Init(textureID, pName, textureType);
}

void TextureImpl::ClearRawData()
{
	m_format = TextureFormat::Count;
	m_rawData.clear();
	m_width = 0;
	m_height = 0;
	m_depth = 0;
}

void TextureImpl::Init(TextureID textureID, std::string name, MaterialTextureType textureType)
{
	m_id = textureID;
	m_name = cd::MoveTemp(name);
	m_type = textureType;

	// Assign default values
	m_uvMapMode[0] = TextureMapMode::Wrap;
	m_uvMapMode[1] = TextureMapMode::Wrap;
	m_uvOffset = cd::Vec2f::Zero();
	m_uvScale = cd::Vec2f::One();

	m_format = TextureFormat::Count;
	m_useMipMap = true;

	ClearRawData();
}

}