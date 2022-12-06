#pragma once

namespace cd
{

static constexpr float PI = 3.1415926535897932f;
static constexpr float HALF_PI = PI / 2.0f;
static constexpr float INV_PI = 1.0f / PI;

// Unfortunately, C++ standard cmath functions are not conexpr. So I can't use std::sqrt(2).
// Consider to rewrite a set of math functios by ourselves or wait C++ 23/26 if not necessary.
static constexpr float SQRT_2 = 1.4142135623730950488016887242097f;
static constexpr float HALF_SQRT_2 = SQRT_2 / 2.0f;
static constexpr float INV_SQRT_2 = 1.0f / SQRT_2;

static constexpr float SQRT_3 = 1.7320508075688772935274463415059f;
static constexpr float HALF_SQRT_3 = SQRT_3 / 2.0f;
static constexpr float INV_SQRT_3 = 1.0f / SQRT_3;

static constexpr float SmallNumberTolerance = 1.e-8f;

}