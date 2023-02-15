#pragma once

#include "Math/Vector.hpp"

#include <optional>
#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cd
{

class PropertyMap final
{
public:
	using KeyType = std::string;

public:
	PropertyMap() = default;
	PropertyMap(const PropertyMap &) = delete;
	PropertyMap &operator=(const PropertyMap &) = delete;
	PropertyMap(PropertyMap &&) = default;
	PropertyMap &operator=(PropertyMap &&) = default;
	~PropertyMap() = default;

	template<typename T>
	void Add(const KeyType &key, const T &value);

	template<typename T>
	std::optional<T> Get(const KeyType &key) const;

	template<typename T>
	void Set(const KeyType &key, const T &value);

	void Remove(const KeyType &key);

private:
	template<typename T>
	void SetValue(const KeyType &key, const T &value);

	template<typename T>
	void CheckType() const;

	bool Exist(const KeyType &key) const;

	std::unordered_map<KeyType, std::string> m_stringProperty;
	std::unordered_map<KeyType, uint32_t>    m_byte4Property;
	std::unordered_map<KeyType, uint64_t>    m_byte8Property;
	std::unordered_map<KeyType, cd::Vec3f>   m_byte12Property;
	std::unordered_map<KeyType, cd::Vec4f>   m_byte16Property;

	std::unordered_set<KeyType> m_keySet;
};

template<typename T>
void PropertyMap::Add(const KeyType &key, const T &value)
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
std::optional<T> PropertyMap::Get(const KeyType &key) const
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
		else if constexpr (std::is_same_v<T, cd::Vec4f>)
		{
			return m_byte16Property.at(key);
		}
	}
	else
	{
		printf("PropertyMap key does not exists!\n");
		return std::nullopt;
	}
}

template<typename T>
void PropertyMap::Set(const KeyType &key, const T &value)
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

void PropertyMap::Remove(const KeyType &key)
{
	if (Exist(key))
	{
		m_keySet.erase(key);
		m_stringProperty.erase(key);
		m_byte4Property.erase(key);
		m_byte8Property.erase(key);
		m_byte12Property.erase(key);
		m_byte16Property.erase(key);
	}
	else
	{
		printf("PropertyMap key does not exists!\n");
	}
}

template<typename T>
void PropertyMap::SetValue(const KeyType &key, const T &value)
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
	else if constexpr (std::is_same_v<T, cd::Vec4f>)
	{
		m_byte16Property[key] = value;
	}
}

bool PropertyMap::Exist(const KeyType &key) const
{
	return !!m_keySet.count(key);
}

template<typename T>
void PropertyMap::CheckType() const
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
		std::is_same_v<T, cd::Vec3f> ||
		std::is_same_v<T, cd::Vec4f>) &&
		"PropertyMap unsupport type!"
	);
}

static_assert(sizeof(int) == sizeof(uint32_t));
static_assert(sizeof(float) == sizeof(uint32_t));
static_assert(sizeof(double) == sizeof(uint64_t));
static_assert(sizeof(cd::Vec2f) == sizeof(uint64_t));

} // namespace cd
