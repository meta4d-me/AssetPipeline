#pragma once

#include "ObjectIDTypes.h"

#include <string>

namespace cdtools
{

class Texture final
{
public:
	Texture() = delete;
	explicit Texture(TextureID textureID, std::string texturePath);
	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	~Texture() = default;

	const TextureID& GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

private:
	TextureID m_id;
	std::string m_path;
};

}