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
	std::string texturePath;
	ImportData(fin, texturePath);

	uint32_t textureID;
	ImportData(fin, textureID);

	Init(TextureID(textureID), std::move(texturePath));
}

void Texture::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::string>(fout, GetPath());
	ExportData<uint32_t>(fout, GetID().Data());
}

}