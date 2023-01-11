#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"

#include <string>

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

	const TextureID& GetID() const { return m_id; }
	cd::MaterialTextureType GetType() const { return m_textureType; }
	const std::string& GetPath() const { return m_path; }

	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t textureID;
		uint8_t textureType;
		std::string texturePath;

		inputArchive >> textureID >> textureType >> texturePath;

		Init(TextureID(textureID), static_cast<cd::MaterialTextureType>(textureType), MoveTemp(texturePath));

		return *this;
	}

	template<bool SwapBytesOrder>
	const TextureImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << static_cast<uint8_t>(GetType()) << GetPath();

		return *this;
	}

private:
	TextureID m_id;
	cd::MaterialTextureType m_textureType;
	std::string m_path;
};

}