#pragma once

#include "Core/ISerializable.h"
#include "Math/VectorDerived.hpp"
#include "ObjectIDTypes.h"

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

class Material final : public ISerializable
{
public:
	using TextureIDMap = std::map<MaterialTextureType, TextureID>;

public:
	Material() = delete;
	explicit Material(std::ifstream& fin);
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

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	MaterialID m_id;
	std::string m_name;

	TextureIDMap m_textureIDs;
};

}