#pragma once

#include "Base/Platform.h"

#include <array>
#include <type_traits>

namespace cd
{

// The idea is from UE's MoveTemp. As MoveTemp is only valid for lvalue and non-const objects, it is helpful
// to get warings when compiler find out wrong usages to avoid performance loss.
template<typename T>
[[nodiscard]] constexpr std::remove_reference_t<T>&& MoveTemp(T&& value) noexcept
{
	static_assert(std::is_lvalue_reference_v<T>, "T is lvalue reference object.");
	static_assert(!std::is_const_v<T>, "For a const object, MoveTemp doesn't take effect.");
	return static_cast<std::remove_reference_t<T>&&>(value);
}

// Generate names for enum automatically.
#define DEFINE_ENUM_WITH_NAMES(EnumName, ...) \
	enum class EnumName { __VA_ARGS__, Count }; \
	constexpr std::array<const char*, static_cast<int>(EnumName::Count)> EnumName##Names = { #__VA_ARGS__ }; \
	constexpr const char* Get##EnumName##Name(EnumName enumValue) { return EnumName##Names[static_cast<int>(enumValue)]; }

}