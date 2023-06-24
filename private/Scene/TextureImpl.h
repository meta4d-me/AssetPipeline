#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"
#include "Scene/TextureFormat.h"

#include <string>
#include <vector>

namespace cd
{

class TextureImpl final
{
public:
	TextureImpl() = delete;
	template<bool SwapBytesOrder>
	explicit TextureImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit TextureImpl(TextureID textureID, const char* pName, MaterialTextureType textureType);
	TextureImpl(const TextureImpl&) = default;
	TextureImpl& operator=(const TextureImpl&) = default;
	TextureImpl(TextureImpl&&) = default;
	TextureImpl& operator=(TextureImpl&&) = default;
	~TextureImpl() = default;

	void Init(TextureID textureID, std::string name, MaterialTextureType textureType);

	TextureID GetID() const { return m_id; }

	const std::string& GetName() const { return m_name; }
	std::string& GetName() { return m_name; }
	void SetName(std::string name) { m_name = MoveTemp(name); }

	cd::MaterialTextureType GetType() const { return m_type; }

	// Texture sampler data
	cd::TextureMapMode GetUMapMode() const { return m_uvMapMode[0]; }
	void SetUMapMode(cd::TextureMapMode mapMode) { m_uvMapMode[0] = mapMode; }

	cd::TextureMapMode GetVMapMode() const { return m_uvMapMode[1]; }
	void SetVMapMode(cd::TextureMapMode mapMode) { m_uvMapMode[1] = mapMode; }

	const cd::Vec2f& GetUVOffset() const { return m_uvOffset; }
	cd::Vec2f& GetUVOffset() { return m_uvOffset; }
	void SetUVOffset(cd::Vec2f uvOffset) { m_uvOffset = cd::MoveTemp(uvOffset); }

	const cd::Vec2f& GetUVScale() const { return m_uvScale; }
	cd::Vec2f& GetUVScale() { return m_uvScale; }
	void SetUVScale(cd::Vec2f uvScale) { m_uvScale = cd::MoveTemp(uvScale); }

	// File texture data
	const std::string& GetPath() const { return m_path; }
	std::string& GetPath() { return m_path; }
	void SetPath(std::string filePath) { m_path = MoveTemp(filePath); }

	// Texture performance data
	bool UseMipMap() const { return m_useMipMap; }
	void SetUseMipMap(bool use) { m_useMipMap = use; }

	void SetFormat(cd::TextureFormat format) { m_format = format; }
	const cd::TextureFormat GetFormat() const { return m_format; }

	// Detailed texture data
	uint32_t GetWidth() const { return m_width; }
	uint32_t& GetWidth() { return m_width; }
	void SetWidth(uint32_t width) { m_width = width; }

	uint32_t GetHeight() const { return m_height; }
	uint32_t& GetHeight() { return m_height; }
	void SetHeight(uint32_t height) { m_height = height; }

	uint32_t GetDepth() const { return m_depth; }
	uint32_t& GetDepth() { return m_depth; }
	void SetDepth(uint32_t depth) { m_depth = depth; }

	const std::vector<std::byte>& GetRawData() const { return m_rawData; }
	void SetRawData(std::vector<std::byte> rawData) { m_rawData = cd::MoveTemp(rawData); }
	void ClearRawData();
	bool ExistRawData() const { return !m_rawData.empty(); }

	// Serialization
	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t textureID;
		std::string name;
		uint8_t textureType;
		inputArchive >> textureID >> name >> textureType;
		Init(TextureID(textureID), cd::MoveTemp(name), static_cast<cd::MaterialTextureType>(textureType));

		uint8_t textureUMapMode;
		uint8_t textureVMapMode;
		inputArchive >> textureUMapMode >> textureVMapMode >> GetUVOffset() >> GetUVScale();
		SetUMapMode(static_cast<cd::TextureMapMode>(textureUMapMode));
		SetVMapMode(static_cast<cd::TextureMapMode>(textureVMapMode));

		uint32_t textureFormat;
		bool useMipmap;
		inputArchive >> textureFormat >> useMipmap;
		SetFormat(static_cast<TextureFormat>(textureFormat));
		SetUseMipMap(useMipmap);

		uint32_t rawDataSize;
		inputArchive >> GetPath() >> GetWidth() >> GetHeight() >> GetDepth() >> rawDataSize;
		m_rawData.resize(rawDataSize);
		inputArchive.ImportBuffer(m_rawData.data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TextureImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint8_t>(GetType()) <<
			static_cast<uint8_t>(GetUMapMode()) << static_cast<uint8_t>(GetVMapMode()) << GetUVOffset() << GetUVScale() <<
			static_cast<uint32_t>(m_format) << UseMipMap();

		outputArchive << GetPath() << GetWidth() << GetHeight() << GetDepth() << m_rawData.size();
		outputArchive.ExportBuffer(m_rawData.data(), m_rawData.size());

		return *this;
	}

private:
	// Texture basic information
	TextureID m_id;
	std::string m_name;
	cd::MaterialTextureType m_type;
	
	// Texture sampler data
	cd::TextureMapMode m_uvMapMode[2];
	cd::Vec2f m_uvOffset;
	cd::Vec2f m_uvScale;

	// Texture performance data
	cd::TextureFormat m_format;
	bool m_useMipMap;

	// File Texture data
	std::string m_path;

	// Detailed Texture data
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;
	std::vector<std::byte> m_rawData;
};

}