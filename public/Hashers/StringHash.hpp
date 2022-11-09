#pragma once

#include <string_view>

namespace cdtools
{

namespace details
{

template<typename T>
struct Fnv1aTraits;

template<>
struct Fnv1aTraits<std::uint32_t>
{
	using T = std::uint32_t;
	static constexpr T Offset = 2166136261U;
	static constexpr T Prime = 16777619U;
};

template<>
struct Fnv1aTraits<std::uint64_t>
{
	using T = std::uint64_t;
	static constexpr T Offset = 14695981039346656037ULL;
	static constexpr T Prime = 1099511628211ULL;
};

}

template<typename T>
constexpr T StringHashSeed(T seed, const char* str, std::size_t n) noexcept
{
	using Traits = details::Fnv1aTraits<T>;

	T value = seed;
	for (std::size_t i = 0; i < n; ++i)
	{
		value = (value ^ static_cast<Traits::T>(str[i])) * Traits::Prime;
	}

	return value;
}

template<typename T>
constexpr T StringHash(const char* str, std::size_t n)
{
	using Traits = details::Fnv1aTraits<T>;
	return StringHashSeed(Traits::Offset, str, n);
}

template<typename T>
constexpr T StringHash(std::string_view sv)
{
	return StringHash<T>(sv.data(), sv.size());
}

}