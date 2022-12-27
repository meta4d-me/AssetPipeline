#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "MaterialTextureType.h"
#include "Math/VectorDerived.hpp"
#include "Scene/ObjectID.h"

#include <map>
#include <optional>

namespace cd
{

class MaterialImpl;

class TOOL_API Material final
{
public:
	using TextureIDMap = std::map<MaterialTextureType, TextureID>;

public:
	Material() = delete;
	explicit Material(InputArchive& inputArchive);
	explicit Material(InputArchiveSwapBytes& inputArchive);
	explicit Material(MaterialID materialID, const char* pMaterialName);
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;
	Material(Material&&);
	Material& operator=(Material&&);
	~Material();

	void Init(MaterialID materialID, const char* pMaterialName);

	const MaterialID& GetID() const;
	const char* GetName() const;
	void SetTextureID(MaterialTextureType textureType, TextureID textureID);
	std::optional<TextureID> GetTextureID(MaterialTextureType textureType) const;
	const TextureIDMap& GetTextureIDMap() const;
	bool IsTextureTypeSetup(MaterialTextureType textureType) const;

	Material& operator<<(InputArchive& inputArchive);
	Material& operator<<(InputArchiveSwapBytes& inputArchive);
	const Material& operator>>(OutputArchive& outputArchive) const;
	const Material& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MaterialImpl* m_pMaterialImpl = nullptr;
};

}