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
		m_rawTexture.resize(sizeInBytes);
		std::memcpy(m_rawTexture.data(), inputData.data(), sizeInBytes);
		m_textureFormat = format;
		m_textureWidth = width;
		m_textureHeight = height;
	}
	void ClearRawTexture();
	bool HasRawTexture() const { return !m_rawTexture.empty(); }

	const TextureID& GetID() const { return m_id; }
	cd::MaterialTextureType GetType() const { return m_textureType; }

	cd::TextureMapMode GetUMapMode() const { return m_textureUMapMode; }
	void SetUMapMode(cd::TextureMapMode mapMode) { m_textureUMapMode = mapMode; }

	cd::TextureMapMode GetVMapMode() const { return m_textureVMapMode; }
	void SetVMapMode(cd::TextureMapMode mapMode) { m_textureVMapMode = mapMode; }

	const std::string& GetPath() const { return m_path; }
	void SetPath(std::string filePath) { m_path = MoveTemp(filePath); }

	const cd::TextureFormat GetTextureFormat() const { return m_textureFormat; }
	const std::vector<std::byte>& GetRawTexture() const { return m_rawTexture; }
	uint32_t GetWidth() const { return m_textureWidth; }
	uint32_t GetHeight() const { return m_textureHeight; }

	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t textureID;
		uint8_t textureType;
		uint8_t textureUMapMode;
		uint8_t textureVMapMode;
		std::string texturePath;
		bool hasRawTexture = false;

		inputArchive >> textureID >> textureType >> textureUMapMode >> textureVMapMode >>
			texturePath >> hasRawTexture;
		if (hasRawTexture)
		{
			uint32_t textureFormat;
			size_t rawTextureSize;
			inputArchive >> textureFormat >> rawTextureSize;

			m_textureFormat = static_cast<TextureFormat>(textureFormat);
			m_rawTexture.reserve(rawTextureSize);
			inputArchive.ImportBuffer(m_rawTexture.data());
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
			static_cast<uint8_t>(GetUMapMode()) << static_cast<uint8_t>(GetVMapMode()) << !m_rawTexture.empty();

		if (!m_rawTexture.empty())
		{
			outputArchive << static_cast<uint32_t>(m_textureFormat) << m_rawTexture.size();
			outputArchive.ExportBuffer(m_rawTexture.data(), m_rawTexture.size());
		}

		return *this;
	}

private:
	TextureID m_id;
	cd::MaterialTextureType m_textureType;
	cd::TextureMapMode m_textureUMapMode;
	cd::TextureMapMode m_textureVMapMode;
	std::string m_path;
	cd::TextureFormat m_textureFormat;
	uint32_t m_textureWidth;
	uint32_t m_textureHeight;
	std::vector<std::byte> m_rawTexture;
};

}