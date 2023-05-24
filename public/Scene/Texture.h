#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"
#include "Scene/TextureFormat.h"

#include <vector>

namespace cd
{

class TextureImpl;

class CORE_API Texture final
{
public:
	Texture() = delete;
	explicit Texture(InputArchive& inputArchive);
	explicit Texture(InputArchiveSwapBytes& inputArchive);
	explicit Texture(TextureID textureID, MaterialTextureType textureType, const char* pTexturePath);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;
	~Texture();

	void SetRawTexture(const std::vector<uint32_t>& inputData, const cd::TextureFormat format, uint32_t width, uint32_t height);
	void SetRawTexture(const std::vector<int32_t>& inputData, const cd::TextureFormat format, uint32_t width, uint32_t height);
	void ClearRawTexture();
	bool HasRawTexture() const;

	const TextureID& GetID() const;
	cd::MaterialTextureType GetType() const;

	cd::TextureMapMode GetUMapMode() const;
	void SetUMapMode(cd::TextureMapMode mapMode);

	cd::TextureMapMode GetVMapMode() const;
	void SetVMapMode(cd::TextureMapMode mapMode);

	const cd::Vec2f& GetUVOffset() const;
	void SetUVOffset(cd::Vec2f uvOffset);

	const cd::Vec2f& GetUVScale() const;
	void SetUVScale(cd::Vec2f uvScale);

	const char* GetPath() const;
	void SetPath(const char* pFilePath);

	const cd::TextureFormat GetTextureFormat() const;
	const std::vector<std::byte>& GetRawTexture() const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	Texture& operator<<(InputArchive& inputArchive);
	Texture& operator<<(InputArchiveSwapBytes& inputArchive);
	const Texture& operator>>(OutputArchive& outputArchive) const;
	const Texture& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TextureImpl* m_pTextureImpl = nullptr;
};

}