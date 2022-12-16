#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "ObjectID.h"

#include <string>

namespace cd
{

class Texture final
{
public:
	Texture() = delete;

	template<bool SwapBytesOrder>
	explicit Texture(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit Texture(TextureID textureID, std::string texturePath);

	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	~Texture() = default;

	void Init(TextureID textureID, std::string texturePath);

	const TextureID& GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

	template<bool SwapBytesOrder>
	Texture& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string texturePath;
		uint32_t textureID;
		inputArchive >> texturePath >> textureID;

		Init(TextureID(textureID), MoveTemp(texturePath));

		return *this;
	}

	template<bool SwapBytesOrder>
	const Texture& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetPath() << GetID().Data();

		return *this;
	}

private:
	TextureID m_id;
	std::string m_path;
};

}