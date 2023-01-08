#pragma once

#include "Constants.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace cd
{

// We are using Left hand system now and +y is up.
enum class Handedness
{
	Left,
	Right
};

// We are using column major matrx now.
enum class MatrixMajor
{
	Column,
	Row
};

// It depends on current choosen graphics API.
// DirectX : ZeroToOne
// OpenGL : MinusOneToOne
enum class NDCDepth
{
	ZeroToOne, // [0, 1]
	MinusOneToOne // [-1, 1]
};

template<typename T>
constexpr T DegreeToRadian(float degree) { return degree * MATH_DEGREE_TO_RADIAN; }

template<typename T>
constexpr T RadianToDegree(float radian) { return radian * MATH_RADIAN_TO_DEGREE; }

template<typename T>
constexpr T GetEpsilon()
{
	if constexpr (std::is_same<float, T>())
	{
		return MATH_FLOAT_EPSILON;
	}
	else if constexpr (std::is_same<double, T>())
	{
		return MATH_DOUBLE_EPSILON;
	}
	else
	{
		return static_cast<T>(0);
	}
}

template<typename T>
constexpr bool IsEqualTo(T a, T b)
{
	if constexpr (std::is_same<float, T>())
	{
		return std::fabs(a - b) <= MATH_FLOAT_EPSILON;
	}
	else if constexpr (std::is_same<double, T>())
	{
		return std::abs(a - b) <= MATH_DOUBLE_EPSILON;
	}
	else
	{
		return a == b;
	}
}

template<typename T>
constexpr bool IsEqualToZero(T a) { return IsEqualTo(a, static_cast<T>(0)); }

template<typename T>
constexpr bool IsEqualToOne(T a) { return IsEqualTo(a, static_cast<T>(1)); }

template<typename T>
constexpr bool IsSmallThan(T a, T b)
{
	if constexpr (std::is_same<float, T>())
	{
		return a + MATH_FLOAT_EPSILON < b;
	}
	else if constexpr (std::is_same<double, T>())
	{
		return a + MATH_DOUBLE_EPSILON < b;
	}
	else
	{
		return a < b;
	}
}

template<typename T>
constexpr bool IsSmallThanZero(T a) { return IsSmallThan(a, static_cast<T>(0)); }

template<typename T>
constexpr bool IsSmallThanOne(T a) { return IsSmallThan(a, static_cast<T>(1)); }

template<typename T>
constexpr bool IsLargeThan(T a, T b)
{
	if constexpr (std::is_same<float, T>())
	{
		return a > b + MATH_FLOAT_EPSILON;
	}
	else if constexpr (std::is_same<double, T>())
	{
		return a > b + MATH_DOUBLE_EPSILON;
	}
	else
	{
		return a > b;
	}
}

template<typename T>
constexpr bool IsLargeThanZero(T a) { return IsLargeThan(a, static_cast<T>(0)); }

template<typename T>
constexpr bool IsLargeThanOne(T a) { return IsLargeThan(a, static_cast<T>(1)); }

}