#include "Texture.h"

namespace cdtools
{

Texture::Texture(std::ifstream& fin)
{
	ImportBinary(fin);
}

Texture::Texture(TextureID textureID, std::string texturePath)
{
	Init(textureID, std::move(texturePath));
}

void Texture::Init(TextureID textureID, std::string texturePath)
{
	m_id = textureID;
	m_path = std::move(texturePath);
}

void Texture::ImportBinary(std::ifstream& fin)
{
	size_t texturePathLength;
	std::string texturePath;
	fin.read(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
	texturePath.resize(texturePathLength);
	fin.read(const_cast<char*>(texturePath.data()), texturePathLength);

	uint32_t textureID;
	fin.read(reinterpret_cast<char*>(&textureID), sizeof(textureID));

	Init(TextureID(textureID), std::move(texturePath));
}

void Texture::ExportBinary(std::ofstream& fout) const
{
	size_t texturePathLength = GetPath().size();
	fout.write(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
	fout.write(GetPath().c_str(), texturePathLength);

	uint32_t textureID = GetID().Data();
	fout.write(reinterpret_cast<char*>(&textureID), sizeof(textureID));
}

}