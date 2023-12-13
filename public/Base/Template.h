#pragma once

#include "Base/Platform.h"

#include <type_traits>

namespace cd
{

// The idea is from UE's MoveTemp. As MoveTemp is only valid for lvalue and non-const objects, it is helpful
// to get warings when compiler find out wrong usages to avoid performance loss.
template<typename T>
[[nodiscard]] constexpr std::remove_reference_t<T>&& MoveTemp(T&& value) noexcept
{
	using CastType = std::remove_reference_t<T>;
	static_assert(std::is_lvalue_reference_v<T>, "T is lvalue reference object.");
	static_assert(!std::is_same_v<CastType&, const CastType&>, "For a const object, MoveTemp doesn't take effect.");
	return static_cast<CastType&&>(value);
}

}