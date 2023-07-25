#pragma once

#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace cd
{

class PropertyMap final
{
public:
	using PropertyMapKeyType = std::string;

public:
	PropertyMap() = default;
	PropertyMap(const PropertyMap &) = delete;
	PropertyMap &operator=(const PropertyMap &) = delete;
	PropertyMap(PropertyMap &&) = default;
	PropertyMap &operator=(PropertyMap &&) = default;
	~PropertyMap() = default;

	template<typename T>
	void Set(const PropertyMapKeyType &key, const T &value)
	{
		CheckType<T>();
		SetValue(key, value);
		m_keySet.insert(key);
	}

	template<typename T>
	const std::optional<T> Get(const PropertyMapKeyType &key) const
	{
		CheckType<T>();
		if (Exist(key))
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				return m_stringProperty.at(key);
			}
			else if constexpr (4 <= sizeof(T))
			{
				return reinterpret_cast<const T &>(m_byte4Property.at(key));
			}
			else if constexpr (8 == sizeof(T))
			{
				return reinterpret_cast<const T &>(m_byte8Property.at(key));
			}
			else if constexpr (12 == sizeof(T))
			{
				return m_byte12Property.at(key);
			}
			else
			{
				static_assert("Overflows the max byte limit.");
			}
		}
		
		return std::nullopt;
	}

	bool Exist(const PropertyMapKeyType &key) const
	{
		return !!m_keySet.count(key);
	}

	void Remove(const PropertyMapKeyType &key)
	{
		if (Exist(key))
		{
			m_keySet.erase(key);
			m_stringProperty.erase(key);
			m_byte4Property.erase(key);
			m_byte8Property.erase(key);
			m_byte12Property.erase(key);
		}
		else
		{
			printf("PropertyMap key does not exists!\n");
		}
	}

	void Clear()
	{
		m_keySet.clear();
		m_stringProperty.clear();
		m_byte4Property.clear();
		m_byte8Property.clear();
		m_byte12Property.clear();
	}

	const std::map<PropertyMapKeyType, std::string> &GetStringProperty() const { return m_stringProperty; }
	const std::map<PropertyMapKeyType, uint32_t> &GetByte4Property() const { return m_byte4Property; }
	const std::map<PropertyMapKeyType, uint64_t> &GetByte8Property() const { return m_byte8Property; }
	const std::map<PropertyMapKeyType, cd::Vec3f> &GetByte12Property() const { return m_byte12Property; }
	const std::set<PropertyMapKeyType> &GetKeySetProperty() const { return m_keySet; }

	template<bool SwapBytesOrder>
	PropertyMap& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint16_t stringCount, byte4Count, byte8Count, byte12Count;
		inputArchive >> stringCount >> byte4Count >> byte8Count >> byte12Count;
		for (uint16_t index = 0; index < stringCount; ++index)
		{
			PropertyMapKeyType key;
			std::string value;
			inputArchive >> key >> value;
			Set(key, value);
		}

		for (uint16_t index = 0; index < byte4Count; ++index)
		{
			PropertyMapKeyType key;
			uint32_t value;
			inputArchive >> key >> value;
			Set(key, value);
		}

		for (uint16_t index = 0; index < byte8Count; ++index)
		{
			PropertyMapKeyType key;
			uint64_t value;
			inputArchive >> key >> value;
			Set(key, value);
		}

		for (uint16_t index = 0; index < byte12Count; ++index)
		{
			PropertyMapKeyType key;
			cd::Vec3f value;
			inputArchive >> key >> value;
			Set(key, value);
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const PropertyMap& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		const auto& stringProperty = GetStringProperty();
		const auto& byte4Property = GetByte4Property();
		const auto& byte8Property = GetByte8Property();
		const auto& byte12Property = GetByte12Property();

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
	template<typename T>
	void SetValue(const PropertyMapKeyType &key, const T &value)
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			m_stringProperty[key] = value;
		}
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float>)
		{
			m_byte4Property[key] = reinterpret_cast<const uint32_t &>(value);
		}
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, cd::Vec2f>)
		{
			m_byte8Property[key] = reinterpret_cast<const uint64_t &>(value);
		}
		else if constexpr (std::is_same_v<T, cd::Vec3f>)
		{
			m_byte12Property[key] = value;
		}
	}

	template<typename T>
	void CheckType() const
	{
		static_assert((
			std::is_same_v<T, std::string> ||
			std::is_same_v<T, int32_t> ||
			std::is_same_v<T, uint32_t> ||
			std::is_same_v<T, bool> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, int64_t> ||
			std::is_same_v<T, uint64_t> ||
			std::is_same_v<T, cd::Vec2f> ||
			std::is_same_v<T, cd::Vec3f>) &&
			"PropertyMap unsupport type!"
			);
	}

private:
	std::map<PropertyMapKeyType, std::string> m_stringProperty;
	std::map<PropertyMapKeyType, uint32_t>    m_byte4Property;
	std::map<PropertyMapKeyType, uint64_t>    m_byte8Property;
	std::map<PropertyMapKeyType, cd::Vec3f>   m_byte12Property;

	std::set<PropertyMapKeyType> m_keySet;
};

static_assert(sizeof(int) == sizeof(uint32_t));
static_assert(sizeof(float) == sizeof(uint32_t));
static_assert(sizeof(double) == sizeof(uint64_t));
static_assert(sizeof(cd::Vec2f) == sizeof(uint64_t));

}
