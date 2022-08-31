#pragma once

#include "ObjectIDTypes.h"
#include "Math/Vector4.h"

#include <map>
#include <optional>
#include <string>

namespace cdtools
{

enum class MaterialTextureType
{
	BaseColor = 0,
	Normal,
	Metalness,
	Roughness,
	Unknown,
};

class Material final
{
public:
	using TextureIDMap = std::map<MaterialTextureType, TextureID>;

public:
	Material() = delete;
	explicit Material(MaterialID materialID, std::string materialName);
	Material(const Material&) = default;
	Material& operator=(const Material&) = default;
	Material(Material&&) = default;
	Material& operator=(Material&&) = default;
	~Material() = default;

	const MaterialID& GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	void SetTextureID(MaterialTextureType textureType, TextureID textureID);
	std::optional<TextureID> GetTextureID(MaterialTextureType textureType) const;
	const TextureIDMap& GetTextureIDMap() const { return m_textureIDs; }

private:
	MaterialID m_id;
	std::string m_name;

	TextureIDMap m_textureIDs;
};

}