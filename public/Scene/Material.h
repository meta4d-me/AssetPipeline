#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "MaterialTextureType.h"
#include "Math/Vector.hpp"
#include "PropertyMap/PropertyMap.hpp"
#include "Scene/ObjectID.h"

#include <map>
#include <optional>

namespace cd
{

class MaterialImpl;

class CORE_API Material final
{
public:
	static const char* GetClassName() { return "Material"; }

public:
	Material() = delete;
	explicit Material(InputArchive& inputArchive);
	explicit Material(InputArchiveSwapBytes& inputArchive);
	explicit Material(MaterialID materialID, const char* pMaterialName, MaterialType type);
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;
	Material(Material&&);
	Material& operator=(Material&&);
	~Material();

	void Init(MaterialID materialID, const char* pMaterialName, MaterialType type);

	const MaterialID& GetID() const;
	const char* GetName() const;

	void AddTextureID(MaterialPropertyGroup textureType, TextureID textureID);
	std::optional<TextureID> GetTextureID(MaterialPropertyGroup textureType) const;
	const PropertyMap &GetPropertyGroups() const;
	bool IsTextureSetup(MaterialPropertyGroup textureType) const;

	Material& operator<<(InputArchive& inputArchive);
	Material& operator<<(InputArchiveSwapBytes& inputArchive);
	const Material& operator>>(OutputArchive& outputArchive) const;
	const Material& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MaterialImpl* m_pMaterialImpl = nullptr;
};

}