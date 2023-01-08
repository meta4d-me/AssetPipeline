#pragma once

#include "Box.hpp"

namespace cd
{

using AABB = TBox<float>;

static_assert(6 * sizeof(float) == sizeof(AABB));
static_assert(std::is_standard_layout_v<AABB>&& std::is_trivial_v<AABB>);

}