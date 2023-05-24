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

Texture::Texture(TextureID textureID, MaterialTextureType textureType, const char* pTexturePath)
{
    m_pTextureImpl = new TextureImpl(textureID, textureType, pTexturePath);
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

void Texture::SetRawTexture(const std::vector<uint32_t>& inputData, const cd::TextureFormat format, uint32_t width, uint32_t height)
{
    m_pTextureImpl->SetRawTexture(inputData, format, width, height);
}

void Texture::SetRawTexture(const std::vector<int32_t>& inputData, const cd::TextureFormat format, uint32_t width, uint32_t height)
{
    m_pTextureImpl->SetRawTexture(inputData, format, width, height);
}

void Texture::ClearRawTexture()
{
    m_pTextureImpl->ClearRawTexture();
}

bool Texture::HasRawTexture() const {
    return m_pTextureImpl->HasRawTexture();
}

const TextureID& Texture::GetID() const
{
    return m_pTextureImpl->GetID();
}

cd::MaterialTextureType Texture::GetType() const
{
    return m_pTextureImpl->GetType();
}

cd::TextureMapMode Texture::GetUMapMode() const
{
    return m_pTextureImpl->GetUMapMode();
}

void Texture::SetUMapMode(cd::TextureMapMode mapMode)
{
    m_pTextureImpl->SetUMapMode(mapMode);
}

cd::TextureMapMode Texture::GetVMapMode() const
{
    return m_pTextureImpl->GetVMapMode();
}

void Texture::SetVMapMode(cd::TextureMapMode mapMode)
{
    m_pTextureImpl->SetVMapMode(mapMode);
}

const cd::Vec2f& Texture::GetUVOffset() const
{
    return m_pTextureImpl->GetUVOffset();
}

void Texture::SetUVOffset(cd::Vec2f uvOffset)
{
    m_pTextureImpl->SetUVOffset(cd::MoveTemp(uvOffset));
}

const cd::Vec2f& Texture::GetUVScale() const
{
    return m_pTextureImpl->GetUVScale();
}

void Texture::SetUVScale(cd::Vec2f uvScale)
{
    m_pTextureImpl->SetUVScale(cd::MoveTemp(uvScale));
}

const char* Texture::GetPath() const
{
    return m_pTextureImpl->GetPath().c_str();
}

void Texture::SetPath(const char* pFilePath)
{
    return m_pTextureImpl->SetPath(pFilePath);
}

const cd::TextureFormat Texture::GetTextureFormat() const
{
    return m_pTextureImpl->GetTextureFormat();
}

const std::vector<std::byte>& Texture::GetRawTexture() const
{
    return m_pTextureImpl->GetRawTexture();
}

uint32_t Texture::GetWidth() const {
    return m_pTextureImpl->GetWidth();
}

uint32_t Texture::GetHeight() const {
    return m_pTextureImpl->GetHeight();
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