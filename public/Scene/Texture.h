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

	void Init(TextureID textureID, MaterialTextureType textureType, const char* pTexturePath);

	void SetRawTexture(const std::vector<int32_t>& inputData, const cd::TextureFormat format);

	void ClearRawTexture();

	const TextureID& GetID() const;
	cd::MaterialTextureType GetType() const;
	const char* GetPath() const;
	const cd::TextureFormat GetTextureFormat() const;
	const std::vector<std::byte>& GetRawTexture() const;

	Texture& operator<<(InputArchive& inputArchive);
	Texture& operator<<(InputArchiveSwapBytes& inputArchive);
	const Texture& operator>>(OutputArchive& outputArchive) const;
	const Texture& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TextureImpl* m_pTextureImpl = nullptr;
};

}