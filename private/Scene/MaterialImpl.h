#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Vector.hpp"
#include "PropertyMap/PropertyMap.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/Types.h"

#include <map>
#include <optional>
#include <string>

namespace cd
{

class MaterialImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Material);

	explicit MaterialImpl(MaterialID materialID, std::string materialName, MaterialType materialType);
	void Init(MaterialID materialID, std::string materialName, MaterialType materialType);
	void InitBasePBR();

	IMPLEMENT_SIMPLE_TYPE_APIS(Material, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Material, Type);
	IMPLEMENT_STRING_TYPE_APIS(Material, Name);

	PropertyMap& GetPropertyGroups() { return m_propertyGroups; }
	const PropertyMap& GetPropertyGroups() const { return m_propertyGroups; }

	void RemoveProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property)
	{
		m_propertyGroups.Remove(GetMaterialPropertyKey(propertyGroup, property));
	}

	template<typename T>
	void SetProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const T& value)
	{
		m_propertyGroups.Set(GetMaterialPropertyKey(propertyGroup, property), value);
	}

	template<typename T>
	std::optional<T> GetProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
	{
		return m_propertyGroups.Get<T>(GetMaterialPropertyKey(propertyGroup, property));
	}

	bool ExistProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property) const
	{
		return m_propertyGroups.Exist(GetMaterialPropertyKey(propertyGroup, property));
	}

	template<bool SwapBytesOrder>
	MaterialImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t materialID;
		std::string materialName;
		uint8_t materialType;
		inputArchive >> materialID >> materialName >> materialType;
		Init(MaterialID(materialID), MoveTemp(materialName), MaterialType(materialType));

		GetPropertyGroups() << inputArchive;

		return *this;
	}

	template<bool SwapBytesOrder>
	const MaterialImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint8_t>(GetType());
		GetPropertyGroups() >> outputArchive;

		return *this;
	}

private:
	PropertyMap m_propertyGroups;
};

}