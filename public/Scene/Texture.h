#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"

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
	Texture(Texture&&);
	Texture& operator=(Texture&&);
	~Texture();

	void Init(TextureID textureID, MaterialTextureType textureType, const char* pTexturePath);

	const TextureID& GetID() const;
	cd::MaterialTextureType GetType() const;
	const char* GetPath() const;

	Texture& operator<<(InputArchive& inputArchive);
	Texture& operator<<(InputArchiveSwapBytes& inputArchive);
	const Texture& operator>>(OutputArchive& outputArchive) const;
	const Texture& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TextureImpl* m_pTextureImpl = nullptr;
};

}