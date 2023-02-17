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
	explicit MaterialImpl(MaterialID materialID, std::string materialName);
	MaterialImpl(const MaterialImpl&) = default;
	MaterialImpl& operator=(const MaterialImpl&) = default;
	MaterialImpl(MaterialImpl&&) = default;
	MaterialImpl& operator=(MaterialImpl&&) = default;
	~MaterialImpl() = default;

	void Init(MaterialID materialID, std::string materialName);
	void SetPropertyDefaultValue();

	const MaterialID& GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }

	void SetTextureID(MaterialPropretyGroup propretyGroup, TextureID textureID);
	std::optional<TextureID> GetTextureID(MaterialPropretyGroup propretyGroup) const;
	const PropertyMap &GetMaterialType() const { return m_basePBRMaterialType; }
	bool IsTextureTypeSetup(MaterialPropretyGroup propretyGroup) const;

	template<bool SwapBytesOrder>
	MaterialImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t materialID;
		std::string materialName;
		inputArchive >> materialID >> materialName;
		Init(MaterialID(materialID), MoveTemp(materialName));

		uint64_t stringCount, byte4Count, byte8Count, byte12Count;
		inputArchive >> stringCount >> byte4Count >> byte8Count >> byte12Count;

		for (uint64_t index = 0; index < stringCount; ++index)
		{
			PropertyMapKeyType key;
			std::string value;
			inputArchive >> key >> value;
			m_basePBRMaterialType.Add(key, value);
		}
		for (uint64_t index = 0; index < byte4Count; ++index)
		{
			PropertyMapKeyType key;
			uint32_t value;
			inputArchive >> key >> value;
			m_basePBRMaterialType.Add(key, value);
		}
		for (uint64_t index = 0; index < byte8Count; ++index)
		{
			PropertyMapKeyType key;
			uint64_t value;
			inputArchive >> key >> value;
			m_basePBRMaterialType.Add(key, value);
		}
		for (uint64_t index = 0; index < byte12Count; ++index)
		{
			PropertyMapKeyType key;
			cd::Vec3f value;
			inputArchive >> key >> value;
			m_basePBRMaterialType.Add(key, value);
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const MaterialImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName();

		const PropertyMap &materailType = GetMaterialType();
		const auto stringProperty       = materailType.GetStringProperty();
		const auto byte4Property        = materailType.GetByte4Property();
		const auto byte8Property        = materailType.GetByte8Property();
		const auto byte12Property       = materailType.GetByte12Property();

		outputArchive <<
			static_cast<uint64_t>(stringProperty.size()) <<
			static_cast<uint64_t>(byte4Property.size()) <<
			static_cast<uint64_t>(byte8Property.size()) <<
			static_cast<uint64_t>(byte12Property.size());
		
		for (const auto &[key, value] : stringProperty)
		{
			outputArchive << key << value;
		}
		for (const auto &[key, value] : byte4Property)
		{
			outputArchive << key << value;
		}
		for (const auto &[key, value] : byte8Property)
		{
			outputArchive << key << value;
		}
		for (const auto &[key, value] : byte12Property)
		{
			outputArchive << key << value;
		}

		return *this;
	}

private:
	MaterialID m_id;
	std::string m_name;

	PropertyMap m_basePBRMaterialType;
};

}