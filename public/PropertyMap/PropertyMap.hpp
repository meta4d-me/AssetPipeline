#pragma once

#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"

#include <optional>
#include <cstdint>
#include <string>
#include <type_traits>
#include <map>
#include <set>
#include <vector>

namespace cd {

namespace {
using PropertyMapKeyType = std::string;
}

class PropertyMap final
{
public:
	PropertyMap() = default;
	PropertyMap(const PropertyMap &) = delete;
	PropertyMap &operator=(const PropertyMap &) = delete;
	PropertyMap(PropertyMap &&) = default;
	PropertyMap &operator=(PropertyMap &&) = default;
	~PropertyMap() = default;

	template<typename T>
	void Add(const PropertyMapKeyType &key, const T &value)
	{
		CheckType<T>();
		if (!Exist(key))
		{
			SetValue(key, value);
			m_keySet.insert(key);
		}
		else
		{
			printf("PropertyMap key alerady exists!\n");
		}
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
			else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float>)
			{
				return reinterpret_cast<const T &>(m_byte4Property.at(key));
			}
			else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, cd::Vec2f>)
			{
				return reinterpret_cast<const T &>(m_byte8Property.at(key));
			}
			else if constexpr (std::is_same_v<T, cd::Vec3f>)
			{
				return m_byte12Property.at(key);
			}
		}
		else
		{
			return std::nullopt;
		}
	}

	template<typename T>
	void Set(const PropertyMapKeyType &key, const T &value)
	{
		CheckType<T>();
		if (Exist(key))
		{
			SetValue(key, value);
		}
		else
		{
			printf("PropertyMap key does not exists!\n");
		}
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
			std::is_same_v<T, uint32_t> ||
			std::is_same_v<T, bool> ||
			std::is_same_v<T, int> ||
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, uint64_t> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, cd::Vec2f> ||
			std::is_same_v<T, cd::Vec3f>) &&
			"PropertyMap unsupport type!"
			);
	}

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

} // namespace cd
