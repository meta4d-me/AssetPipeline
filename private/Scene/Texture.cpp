#include "Scene/Texture.h"
#include "TextureImpl.h"

namespace cd
{

Texture::Texture(InputArchive& inputArchive)
{
    m_pTextureImpl = new TextureImpl(inputArchive);
}

Texture::Texture(InputArchiveSwapBytes& inputArchive)
{
    m_pTextureImpl = new TextureImpl(inputArchive);
}

Texture::Texture(TextureID textureID, const char* pTexturePath)
{
    m_pTextureImpl = new TextureImpl(textureID, pTexturePath);
}

Texture::Texture(Texture&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Texture& Texture::operator=(Texture&& rhs)
{
    std::swap(m_pTextureImpl, rhs.m_pTextureImpl);
    return *this;
}

Texture::~Texture()
{
    if (m_pTextureImpl)
    {
        delete m_pTextureImpl;
        m_pTextureImpl = nullptr;
    }
}

void Texture::Init(TextureID textureID, const char* pTexturePath)
{
    m_pTextureImpl->Init(textureID, pTexturePath);
}

const TextureID& Texture::GetID() const
{
    return m_pTextureImpl->GetID();
}

const char* Texture::GetPath() const
{
    return m_pTextureImpl->GetPath().c_str();
}

Texture& Texture::operator<<(InputArchive& inputArchive)
{
    *m_pTextureImpl << inputArchive;
    return *this;
}

Texture& Texture::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pTextureImpl << inputArchive;
    return *this;
}

const Texture& Texture::operator>>(OutputArchive& outputArchive) const
{
    *m_pTextureImpl >> outputArchive;
    return *this;
}

const Texture& Texture::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pTextureImpl >> outputArchive;
    return *this;
}

}