#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "MaterialTextureType.h"
#include "Math/Vector.hpp"
#include "PropertyMap/PropertyMap.hpp"
#include "Scene/Types.h"

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

	EXPORT_SIMPLE_TYPE_APIS(Material, ID);
	EXPORT_SIMPLE_TYPE_APIS(Material, Type);
	EXPORT_STRING_TYPE_APIS(Material, Name);
	
	const PropertyMap& GetPropertyGroups() const;

	void SetTextureID(MaterialPropertyGroup textureType, TextureID textureID);
	TextureID GetTextureID(MaterialPropertyGroup textureType) const;
	void RemoveTexture(MaterialPropertyGroup textureType);
	bool IsTextureSetup(MaterialPropertyGroup textureType) const;

	void RemoveProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property);

	void SetBoolProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, bool value);
	void SetI32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, int32_t value);
	void SetI64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, int64_t value);
	void SetU32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, uint32_t value);
	void SetU64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property, uint64_t value);
	void SetFloatProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, float value);
	void SetDoubleProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, double value);
	void SetStringProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const std::string& value);
	void SetVec2fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const cd::Vec2f& value);
	void SetVec3fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const cd::Vec3f& value);

	std::optional<bool> GetBoolProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<int32_t> GetI32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<int64_t> GetI64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<uint32_t> GetU32Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<uint64_t> GetU64Property(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<float> GetFloatProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<double> GetDoubleProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<std::string> GetStringProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;
	std::optional<cd::Vec2f> GetVec2fProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;

	bool ExistProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const;

	Material& operator<<(InputArchive& inputArchive);
	Material& operator<<(InputArchiveSwapBytes& inputArchive);
	const Material& operator>>(OutputArchive& outputArchive) const;
	const Material& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MaterialImpl* m_pMaterialImpl = nullptr;
};

}