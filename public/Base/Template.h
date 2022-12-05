#pragma once

#include <type_traits>

namespace cd
{

// The idea is from UE's cd::MoveTemp. As cd::MoveTemp is only valid for lvalue and non-const objects, it is helpful
// to get warings when compiler find out wrong usages to avoid performance loss.
template<typename T>
[[nodiscard]] constexpr std::remove_reference_t<T>&& MoveTemp(T&& value) noexcept
{
	static_assert(std::is_lvalue_reference_v<T>, "For an rvalue object, don't need to call MoveTemp.");
	static_assert(!std::is_const_v<T>, "For a const object, MoveTemp doesn't take effect.");
	return static_cast<std::remove_reference_t<T>&&>(value);
}

}