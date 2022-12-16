#include "Texture.h"

namespace cd
{

Texture::Texture(TextureID textureID, std::string texturePath)
{
	Init(textureID, MoveTemp(texturePath));
}

void Texture::Init(TextureID textureID, std::string texturePath)
{
	m_id = textureID;
	m_path = MoveTemp(texturePath);
}

}