#pragma once

#include <cassert>
#include <string>

namespace cd
{

template<typename T, typename ... Args>
std::basic_string<T> string_format(const T* format, Args ... args)
{
	int size_signed = 0;
	// Determine size first
	if constexpr (std::is_same_v<T, char>) 
	{
		// char
		size_signed = std::snprintf(nullptr, 0, format, args ...);
	}
	else
	{
		// wchar_t
		size_signed = std::swprintf(nullptr, 0, format, args ...);
	}
	assert(size_signed > 0);
	const size_t size = static_cast<size_t>(size_signed);
	std::basic_string<T> formatted(size, T{});
	if constexpr (std::is_same_v<T, char>) {
		// char
		// +1 for the '\0' (it will not be part of formatted).
		std::snprintf(formatted.data(), size + 1, format, args ...);
	}
	else {
		// wchar_t
		// +1 for the '\0' (it will not be part of formatted).
		std::swprintf(formatted.data(), size + 1, format, args ...);
	}
	return formatted;
}

}