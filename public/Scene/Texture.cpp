#include "Texture.h"

namespace cdtools
{

Texture::Texture(TextureID textureID, std::string texturePath) :
	m_id(textureID),
	m_path(std::move(texturePath))
{
}

}