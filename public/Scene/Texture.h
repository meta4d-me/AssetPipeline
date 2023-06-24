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
	static const char* GetClassName() { return "Texture"; }

public:
	Texture() = delete;
	explicit Texture(InputArchive& inputArchive);
	explicit Texture(InputArchiveSwapBytes& inputArchive);
	explicit Texture(TextureID textureID, const char* pName, MaterialTextureType textureType);
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;
	~Texture();

	TextureID GetID() const;
	
	const std::string& GetName() const;
	std::string& GetName();
	void SetName(std::string name);

	cd::MaterialTextureType GetType() const;

	// Texture sampler data
	cd::TextureMapMode GetUMapMode() const;
	void SetUMapMode(cd::TextureMapMode mapMode);

	cd::TextureMapMode GetVMapMode() const;
	void SetVMapMode(cd::TextureMapMode mapMode);

	const cd::Vec2f& GetUVOffset() const;
	void SetUVOffset(cd::Vec2f uvOffset);

	const cd::Vec2f& GetUVScale() const;
	void SetUVScale(cd::Vec2f uvScale);

	// File texture data
	const char* GetPath() const;
	void SetPath(const char* filePath);

	// Texture performance data
	bool UseMipMap() const;
	void SetUseMipMap(bool use);

	void SetFormat(cd::TextureFormat format);
	cd::TextureFormat GetFormat() const;

	// Detailed texture data
	uint32_t GetWidth() const;
	void SetWidth(uint32_t width);

	uint32_t GetHeight() const;
	void SetHeight(uint32_t height);

	uint32_t GetDepth() const;
	void SetDepth(uint32_t depth);

	const std::vector<std::byte>& GetRawData() const;
	void SetRawData(std::vector<std::byte> rawData);
	void ClearRawData();
	bool ExistRawData() const;
	
	Texture& operator<<(InputArchive& inputArchive);
	Texture& operator<<(InputArchiveSwapBytes& inputArchive);
	const Texture& operator>>(OutputArchive& outputArchive) const;
	const Texture& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TextureImpl* m_pTextureImpl = nullptr;
};

}