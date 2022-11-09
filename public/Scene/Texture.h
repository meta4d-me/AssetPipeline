#pragma once

#include "Core/ISerializable.hpp"
#include "ObjectID.h"

#include <string>

namespace cdtools
{

class Texture final : public ISerializable
{
public:
	Texture() = delete;
	explicit Texture(std::ifstream& fin);
	explicit Texture(TextureID textureID, std::string texturePath);
	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;
	~Texture() = default;

	void Init(TextureID textureID, std::string texturePath);

	const TextureID& GetID() const { return m_id; }
	const std::string& GetPath() const { return m_path; }

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	TextureID m_id;
	std::string m_path;
};

}