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

Texture::Texture(TextureID textureID, const char* pName, MaterialTextureType textureType)
{
    m_pTextureImpl = new TextureImpl(textureID, pName, textureType);
}

Texture::Texture(Texture&& rhs) noexcept
{
    *this = cd::MoveTemp(rhs);
}

Texture& Texture::operator=(Texture&& rhs) noexcept
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

PIMPL_SIMPLE_TYPE_APIS(Texture, ID);
PIMPL_SIMPLE_TYPE_APIS(Texture, Type);
PIMPL_SIMPLE_TYPE_APIS(Texture, Format);
PIMPL_SIMPLE_TYPE_APIS(Texture, UMapMode);
PIMPL_SIMPLE_TYPE_APIS(Texture, VMapMode);
PIMPL_SIMPLE_TYPE_APIS(Texture, UseMipMap);
PIMPL_SIMPLE_TYPE_APIS(Texture, Width);
PIMPL_SIMPLE_TYPE_APIS(Texture, Height);
PIMPL_SIMPLE_TYPE_APIS(Texture, Depth);
PIMPL_COMPLEX_TYPE_APIS(Texture, UVOffset);
PIMPL_COMPLEX_TYPE_APIS(Texture, UVScale);
PIMPL_COMPLEX_TYPE_APIS(Texture, RawData);
PIMPL_STRING_TYPE_APIS(Texture, Name);
PIMPL_STRING_TYPE_APIS(Texture, Path);

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