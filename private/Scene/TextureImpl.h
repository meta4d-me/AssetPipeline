#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
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
	explicit TextureImpl(TextureID textureID, std::string texturePath);
	TextureImpl(const TextureImpl&) = default;
	TextureImpl& operator=(const TextureImpl&) = default;
	TextureImpl(TextureImpl&&) = default;
	TextureImpl& operator=(TextureImpl&&) = default;
	~TextureImpl() = default;

	void Init(TextureID textureID, std::string texturePath);

	const TextureID& GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string texturePath;
		uint32_t textureID;
		inputArchive >> texturePath >> textureID;

		Init(TextureID(textureID), MoveTemp(texturePath));

		return *this;
	}

	template<bool SwapBytesOrder>
	const TextureImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetPath() << GetID().Data();

		return *this;
	}

private:
	TextureID m_id;
	std::string m_path;
};

}