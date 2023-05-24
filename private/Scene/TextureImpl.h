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
	explicit TextureImpl(TextureID textureID, MaterialTextureType textureType, std::string texturePath);
	TextureImpl(const TextureImpl&) = default;
	TextureImpl& operator=(const TextureImpl&) = default;
	TextureImpl(TextureImpl&&) = default;
	TextureImpl& operator=(TextureImpl&&) = default;
	~TextureImpl() = default;

	void Init(TextureID textureID, MaterialTextureType textureType, std::string texturePath);

	template <typename T>
	void SetRawTexture(const std::vector<T>& inputData, const cd::TextureFormat format, const uint32_t width, const uint32_t height) {
		const size_t sizeRatio = sizeof(T) / sizeof(std::byte);
		const size_t sizeInBytes = inputData.size() * sizeRatio;
		m_rawData.resize(sizeInBytes);
		std::memcpy(m_rawData.data(), inputData.data(), sizeInBytes);
		m_format = format;
		m_width = width;
		m_height = height;
	}
	void ClearRawTexture();
	bool HasRawTexture() const { return !m_rawData.empty(); }

	const TextureID& GetID() const { return m_id; }
	cd::MaterialTextureType GetType() const { return m_type; }

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

	const std::string& GetPath() const { return m_path; }
	void SetPath(std::string filePath) { m_path = MoveTemp(filePath); }

	const cd::TextureFormat GetTextureFormat() const { return m_format; }
	const std::vector<std::byte>& GetRawTexture() const { return m_rawData; }
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }

	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t textureID;
		uint8_t textureType;
		uint8_t textureUMapMode;
		uint8_t textureVMapMode;
		std::string texturePath;
		bool hasRawTexture = false;

		inputArchive >> textureID >> textureType >> texturePath >> textureUMapMode >> textureVMapMode >>
			GetUVOffset() >> GetUVScale() >> hasRawTexture;
		if (hasRawTexture)
		{
			uint32_t textureFormat;
			size_t rawTextureSize;
			inputArchive >> textureFormat >> rawTextureSize;

			m_format = static_cast<TextureFormat>(textureFormat);
			m_rawData.reserve(rawTextureSize);
			inputArchive.ImportBuffer(m_rawData.data());
		}

		Init(TextureID(textureID), static_cast<cd::MaterialTextureType>(textureType), MoveTemp(texturePath));
		SetUMapMode(static_cast<cd::TextureMapMode>(textureUMapMode));
		SetVMapMode(static_cast<cd::TextureMapMode>(textureVMapMode));

		return *this;
	}

	template<bool SwapBytesOrder>
	const TextureImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << static_cast<uint8_t>(GetType()) << GetPath() <<
			static_cast<uint8_t>(GetUMapMode()) << static_cast<uint8_t>(GetVMapMode()) <<
			GetUVOffset() << GetUVScale() << !m_rawData.empty();

		if (!m_rawData.empty())
		{
			outputArchive << static_cast<uint32_t>(m_format) << m_rawData.size();
			outputArchive.ExportBuffer(m_rawData.data(), m_rawData.size());
		}

		return *this;
	}

private:
	TextureID m_id;
	cd::MaterialTextureType m_type;
	cd::TextureMapMode m_uvMapMode[2];
	cd::Vec2f m_uvOffset;
	cd::Vec2f m_uvScale;
	std::string m_path;
	cd::TextureFormat m_format;
	uint32_t m_width;
	uint32_t m_height;
	std::vector<std::byte> m_rawData;
};

}