#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "MaterialTextureType.h"
#include "Math/VectorDerived.hpp"
#include "ObjectID.h"

#include <map>
#include <optional>
#include <string>

namespace cd
{

class Material final
{
public:
	using TextureIDMap = std::map<MaterialTextureType, TextureID>;

public:
	Material() = delete;

	template<bool SwapBytesOrder>
	explicit Material(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit Material(MaterialID materialID, std::string materialName);

	Material(const Material&) = default;
	Material& operator=(const Material&) = default;
	Material(Material&&) = default;
	Material& operator=(Material&&) = default;
	~Material() = default;

	void Init(MaterialID materialID, std::string materialName);

	const MaterialID& GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	void SetTextureID(MaterialTextureType textureType, TextureID textureID);
	std::optional<TextureID> GetTextureID(MaterialTextureType textureType) const;
	const TextureIDMap& GetTextureIDMap() const { return m_textureIDs; }
	bool IsTextureTypeSetup(MaterialTextureType textureType) const { return m_textureIDs.find(textureType) != m_textureIDs.end(); }

	template<bool SwapBytesOrder>
	Material& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string materialName;
		uint32_t materialID;
		size_t materialTextureCount;
		inputArchive >> materialName >> materialID >> materialTextureCount;

		Init(MaterialID(materialID), MoveTemp(materialName));

		for (uint32_t textureIndex = 0; textureIndex < materialTextureCount; ++textureIndex)
		{
			size_t textureType;
			uint32_t materialTextureID;
			inputArchive >> textureType >> materialTextureID;
			SetTextureID(static_cast<MaterialTextureType>(textureType), TextureID(materialTextureID));
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const Material& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		const Material::TextureIDMap& textureIDMap = GetTextureIDMap();
		outputArchive << GetName() << GetID().Data() << textureIDMap.size();

		for (const auto& [materialTextureType, textureID] : textureIDMap)
		{
			outputArchive << static_cast<size_t>(materialTextureType) << textureID.Data();
		}

		return *this;
	}

private:
	MaterialID m_id;
	std::string m_name;

	TextureIDMap m_textureIDs;
};

}