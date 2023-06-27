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
	MaterialImpl(const MaterialImpl&) = default;
	MaterialImpl& operator=(const MaterialImpl&) = default;
	MaterialImpl(MaterialImpl&&) = default;
	MaterialImpl& operator=(MaterialImpl&&) = default;
	~MaterialImpl() = default;

	void Init(MaterialID materialID, std::string materialName, MaterialType materialType);
	void InitBasePBR();

	MaterialID GetID() const { return m_id; }
	void SetID(MaterialID id) { m_id = id; }

	const std::string& GetName() const { return m_name; }
	void SetName(std::string name) { m_name = cd::MoveTemp(name); }

	MaterialType GetType() const { return m_type; }
	void SetType(MaterialType materialType) { m_type = materialType; }

	const PropertyMap& GetPropertyGroups() const { return m_propertyGroups; }
	
	template<typename T>
	void AddProperty(MaterialPropertyGroup propertyGroup, MaterialProperty property, const T& value)
	{
		m_propertyGroups.Add(GetMaterialPropertyKey(propertyGroup, property), value);
	}

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

		uint16_t stringCount, byte4Count, byte8Count, byte12Count;
		inputArchive >> stringCount >> byte4Count >> byte8Count >> byte12Count;
		for (uint16_t index = 0; index < stringCount; ++index)
		{
			PropertyMapKeyType key;
			std::string value;
			inputArchive >> key >> value;
			m_propertyGroups.Add(key, value);
		}

		for (uint16_t index = 0; index < byte4Count; ++index)
		{
			PropertyMapKeyType key;
			uint32_t value;
			inputArchive >> key >> value;
			m_propertyGroups.Add(key, value);
		}

		for (uint16_t index = 0; index < byte8Count; ++index)
		{
			PropertyMapKeyType key;
			uint64_t value;
			inputArchive >> key >> value;
			m_propertyGroups.Add(key, value);
		}

		for (uint16_t index = 0; index < byte12Count; ++index)
		{
			PropertyMapKeyType key;
			cd::Vec3f value;
			inputArchive >> key >> value;
			m_propertyGroups.Add(key, value);
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const MaterialImpl &operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint8_t>(GetType());

		const auto& groups         = GetPropertyGroups();
		const auto& stringProperty = groups.GetStringProperty();
		const auto& byte4Property  = groups.GetByte4Property();
		const auto& byte8Property  = groups.GetByte8Property();
		const auto& byte12Property = groups.GetByte12Property();

		outputArchive <<
			static_cast<uint16_t>(stringProperty.size()) <<
			static_cast<uint16_t>(byte4Property.size()) <<
			static_cast<uint16_t>(byte8Property.size()) <<
			static_cast<uint16_t>(byte12Property.size());

		for (const auto& [key, value] : stringProperty)
		{
			outputArchive << key << value;
		}
		for (const auto& [key, value] : byte4Property)
		{
			outputArchive << key << value;
		}
		for (const auto& [key, value] : byte8Property)
		{
			outputArchive << key << value;
		}
		for (const auto& [key, value] : byte12Property)
		{
			outputArchive << key << value;
		}

		return *this;
	}

private:
	MaterialID m_id;
	std::string m_name;
	MaterialType m_type;
	
	PropertyMap m_propertyGroups;
};

}