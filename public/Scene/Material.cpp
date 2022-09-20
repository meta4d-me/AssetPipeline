#include "Material.h"

#include <cassert>

namespace cdtools
{

Material::Material(std::ifstream& fin)
{
	ImportBinary(fin);
}

Material::Material(MaterialID materialID, std::string materialName)
{
	Init(materialID, std::move(materialName));
}

void Material::Init(MaterialID materialID, std::string materialName)
{
	m_id = materialID;
	m_name = std::move(materialName);
}

void Material::SetTextureID(MaterialTextureType textureType, TextureID textureID)
{
	TextureIDMap::iterator itTexture = m_textureIDs.find(textureType);
	if(itTexture != m_textureIDs.end())
	{
		// Existed!
		if(textureID == itTexture->second)
		{
			// Same
			return;
		}
	}

	m_textureIDs[textureType] = textureID;
}

std::optional<TextureID> Material::GetTextureID(MaterialTextureType textureType) const
{
	TextureIDMap::const_iterator itTexture = m_textureIDs.find(textureType);
	if(itTexture != m_textureIDs.end())
	{
		return itTexture->second;
	}

	return std::nullopt;
}

void Material::ImportBinary(std::ifstream& fin)
{
	size_t materialNameLength;
	std::string materialName;
	fin.read(reinterpret_cast<char*>(&materialNameLength), sizeof(materialNameLength));
	materialName.resize(materialNameLength);
	fin.read(const_cast<char*>(materialName.data()), materialNameLength);

	uint32_t materialID;
	fin.read(reinterpret_cast<char*>(&materialID), sizeof(materialID));

	Init(MaterialID(materialID), std::move(materialName));

	size_t materialTextureCount;
	fin.read(reinterpret_cast<char*>(&materialTextureCount), sizeof(materialTextureCount));

	for (uint32_t textureIndex = 0; textureIndex < materialTextureCount; ++textureIndex)
	{
		size_t textureType;
		fin.read(reinterpret_cast<char*>(&textureType), sizeof(textureType));

		uint32_t materialTextureID;
		fin.read(reinterpret_cast<char*>(&materialTextureID), sizeof(materialTextureID));

		SetTextureID(static_cast<MaterialTextureType>(textureType), TextureID(materialTextureID));
	}
}

void Material::ExportBinary(std::ofstream& fout) const
{
	size_t materialNameLength = GetName().size();
	fout.write(reinterpret_cast<char*>(&materialNameLength), sizeof(materialNameLength));
	fout.write(GetName().c_str(), materialNameLength);

	uint32_t materialID = GetID().Data();
	fout.write(reinterpret_cast<char*>(&materialID), sizeof(materialID));

	const Material::TextureIDMap& textureIDMap = GetTextureIDMap();
	size_t materialTextureCount = textureIDMap.size();
	fout.write(reinterpret_cast<char*>(&materialTextureCount), sizeof(materialTextureCount));

	for (Material::TextureIDMap::const_iterator it = textureIDMap.begin(), itEnd = textureIDMap.end();
		it != itEnd; ++it)
	{
		size_t textureType = static_cast<size_t>(it->first);
		uint32_t materialTextureID = it->second.Data();

		fout.write(reinterpret_cast<char*>(&textureType), sizeof(textureType));
		fout.write(reinterpret_cast<char*>(&materialTextureID), sizeof(materialTextureID));
	}
}

}