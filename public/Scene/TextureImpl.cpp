#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, std::string texturePath)
{
	Init(textureID, MoveTemp(texturePath));
}

void TextureImpl::Init(TextureID textureID, std::string texturePath)
{
	m_id = textureID;
	m_path = MoveTemp(texturePath);
}

}