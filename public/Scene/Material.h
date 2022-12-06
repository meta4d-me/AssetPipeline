#pragma once

#include "Core/ISerializable.hpp"
#include "MaterialTextureType.h"
#include "Math/VectorDerived.hpp"
#include "ObjectID.h"

#include <map>
#include <optional>
#include <string>

namespace cd
{

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
	bool IsTextureTypeSetup(MaterialTextureType textureType) const { return m_textureIDs.find(textureType) != m_textureIDs.end(); }

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	MaterialID m_id;
	std::string m_name;

	TextureIDMap m_textureIDs;
};

}