#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "PropertyMap/PropertyMap.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"
#include "Math/Vector.hpp"

#include <map>
#include <optional>
#include <string>

namespace cd
{

class MaterialImpl final
{
public:
	MaterialImpl() = delete;
	template<bool SwapBytesOrder>
	explicit MaterialImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit MaterialImpl(MaterialID materialID, std::string materialName, MaterialType materialType);
	MaterialImpl(const MaterialImpl&) = delete;
	MaterialImpl& operator=(const MaterialImpl&) = delete;
	MaterialImpl(MaterialImpl&&) = default;
	MaterialImpl& operator=(MaterialImpl&&) = default;
	~MaterialImpl() = default;

	void Init(MaterialID materialID, std::string materialName, MaterialType materialType);
	void InitBasePBR();

	IMPLEMENT_ID_APIS(MaterialID, m_id);
	IMPLEMENT_NAME_APIS(m_name);

	MaterialType GetType() const { return m_type; }
	void SetType(MaterialType materialType) { m_type = materialType; }

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
	MaterialID m_id;
	std::string m_name;
	MaterialType m_type;
	
	PropertyMap m_propertyGroups;
};

}