#pragma once

#include "Math/Vector.hpp"

#include <optional>
#include <stdint.h>
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
	PropertyMap(const PropertyMap &) = default;
	PropertyMap &operator=(const PropertyMap &) = default;
	PropertyMap(PropertyMap &&) = default;
	PropertyMap &operator=(PropertyMap &&) = default;
	~PropertyMap() = default;

	template<typename T>
	void Add(const KeyType &key, T value);

	template<typename T>
	std::optional<T> Get(const KeyType &key);

	template<typename T>
	void Set(const KeyType &key, T value);

	void Remove(const KeyType &key);

private:
	template<typename T>
	void SetValue(const KeyType &key, T value);

	template<typename T>
	void CheckType();

	bool Exist(const KeyType &key);

	std::unordered_map<KeyType, std::string> m_stringProperty;
	std::unordered_map<KeyType, uint32_t>    m_byte4Property;
	std::unordered_map<KeyType, uint64_t>    m_byte8Property;
	std::unordered_map<KeyType, cd::Vec3f>   m_byte12Property;
	std::unordered_map<KeyType, cd::Vec4f>   m_byte16Property;

	std::unordered_set<KeyType> m_keySet;
};

template<typename T>
void PropertyMap::Add(const KeyType &key, T value)
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
std::optional<T> PropertyMap::Get(const KeyType &key)
{
	CheckType<T>();
	if (Exist(key))
	{
		if constexpr (std::is_same_v<T, const char *>) {
			return m_stringProperty[key].c_str();
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			return m_stringProperty[key];
		}
		else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float>)
		{
			return reinterpret_cast<T &>(m_byte4Property[key]);
		}
		else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, cd::Vec2f>)
		{
			return reinterpret_cast<T &>(m_byte8Property[key]);
		}
		else if constexpr (std::is_same_v<T, cd::Vec3f>)
		{
			return m_byte12Property[key];
		}
		else if constexpr (std::is_same_v<T, cd::Vec4f>)
		{
			return m_byte16Property[key];
		}
	}
	else
	{
		printf("PropertyMap key does not exists!\n");
		std::nullopt;
	}
}

template<typename T>
void PropertyMap::Set(const KeyType &key, T value)
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
void PropertyMap::SetValue(const KeyType &key, T value)
{
	if constexpr (std::is_same_v<T, const char *> || std::is_same_v<T, std::string>)
	{
		m_stringProperty[key] = value;
	}
	else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, bool> || std::is_same_v<T, int> || std::is_same_v<T, float>)
	{
		m_byte4Property[key] = reinterpret_cast<uint32_t &>(value);
	}
	else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, cd::Vec2f>)
	{
		m_byte8Property[key] = reinterpret_cast<uint64_t &>(value);
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

bool PropertyMap::Exist(const KeyType &key)
{
	if (m_keySet.find(key) != m_keySet.end())
	{
		return true;
	}
	return false;
}

template<typename T>
void PropertyMap::CheckType()
{
	static_assert((
		std::is_same_v<T, const char *> ||
		std::is_same_v<T, uint32_t> ||
		std::is_same_v<T, std::string> ||
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
