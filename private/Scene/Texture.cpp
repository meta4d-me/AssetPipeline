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

TextureID Texture::GetID() const
{
    return m_pTextureImpl->GetID();
}

cd::MaterialTextureType Texture::GetType() const
{
    return m_pTextureImpl->GetType();
}

void Texture::SetType(MaterialTextureType type)
{
    m_pTextureImpl->SetType(type);
}

const char* Texture::GetName() const
{
    return m_pTextureImpl->GetName().c_str();
}

void Texture::SetName(const char* pName)
{
    m_pTextureImpl->SetName(pName);
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

cd::TextureFormat Texture::GetFormat() const
{
    return m_pTextureImpl->GetFormat();
}

void Texture::SetFormat(cd::TextureFormat format)
{
    return m_pTextureImpl->SetFormat(format);
}

const char* Texture::GetPath() const
{
    return m_pTextureImpl->GetPath().c_str();
}

void Texture::SetPath(const char* pFilePath)
{
    return m_pTextureImpl->SetPath(pFilePath);
}

bool Texture::UseMipMap() const
{
    return m_pTextureImpl->UseMipMap();
}

void Texture::SetUseMipMap(bool use)
{
    m_pTextureImpl->SetUseMipMap(use);
}

uint32_t Texture::GetWidth() const
{
    return m_pTextureImpl->GetWidth();
}

void Texture::SetWidth(uint32_t width)
{
    m_pTextureImpl->SetWidth(width);
}

uint32_t Texture::GetHeight() const
{
    return m_pTextureImpl->GetHeight();
}

void Texture::SetHeight(uint32_t height)
{
    m_pTextureImpl->SetHeight(height);
}

uint32_t Texture::GetDepth() const
{
    return m_pTextureImpl->GetDepth();
}

void Texture::SetDepth(uint32_t depth)
{
    m_pTextureImpl->SetDepth(depth);
}

const std::vector<std::byte>& Texture::GetRawData() const
{
    return m_pTextureImpl->GetRawData();
}

void Texture::SetRawData(std::vector<std::byte> rawData)
{
    m_pTextureImpl->SetRawData(cd::MoveTemp(rawData));
}

void Texture::ClearRawData()
{
    m_pTextureImpl->ClearRawData();
}

bool Texture::ExistRawData() const
{
    return m_pTextureImpl->ExistRawData();
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