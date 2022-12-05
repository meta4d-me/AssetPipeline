#include "Material.h"

#include <cassert>

namespace cd
{

Material::Material(std::ifstream& fin)
{
	ImportBinary(fin);
}

Material::Material(MaterialID materialID, std::string materialName)
{
	Init(materialID, MoveTemp(materialName));
}

void Material::Init(MaterialID materialID, std::string materialName)
{
	m_id = materialID;
	m_name = MoveTemp(materialName);
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
	std::string materialName;
	ImportData(fin, materialName);

	uint32_t materialID;
	ImportData(fin, materialID);

	Init(MaterialID(materialID), MoveTemp(materialName));

	size_t materialTextureCount;
	ImportData(fin, materialTextureCount);
	for (uint32_t textureIndex = 0; textureIndex < materialTextureCount; ++textureIndex)
	{
		size_t textureType;
		uint32_t materialTextureID;
		ImportData(fin, textureType);
		ImportData(fin, materialTextureID);

		SetTextureID(static_cast<MaterialTextureType>(textureType), TextureID(materialTextureID));
	}
}

void Material::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::string>(fout, GetName());
	ExportData<uint32_t>(fout, GetID().Data());

	const Material::TextureIDMap& textureIDMap = GetTextureIDMap();
	ExportData<size_t>(fout, textureIDMap.size());

	for (const auto& [materialTextureType, textureID] : textureIDMap)
	{
		ExportData<size_t>(fout, static_cast<size_t>(materialTextureType));
		ExportData<uint32_t>(fout, textureID.Data());
	}
}

}