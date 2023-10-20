#pragma once

#include <nameof.hpp>

namespace nameof
{

template <typename E>
[[nodiscard]] constexpr auto enum_count() noexcept
{
	return nameof::detail::count_v<std::decay_t<E>>;
}

}