#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "AxisSystem.hpp"

namespace cd
{

class Math final
{
public:
	Math() = delete;

	static constexpr float FLOAT_EPSILON = 1.192092896e-07F;
	static constexpr float DOUBLE_EPSILON = 2.2204460492503131e-016;

	static constexpr float PI = 3.1415926535897932f;
	static constexpr float HALF_PI = PI / 2.0f;
	static constexpr float TWO_PI = PI * 2.0f;
	static constexpr float INVERSE_PI = 1.0f / PI;
	static constexpr float DEGREE_TO_RADIAN = PI / 180.0f;
	static constexpr float RADIAN_TO_DEGREE = 1 / DEGREE_TO_RADIAN;

	static constexpr float SQRT_2 = 1.4142135623730950488016887242097f;
	static constexpr float HALF_SQRT_2 = SQRT_2 / 2.0f;
	static constexpr float INVERSE_SQRT_2 = 1.0f / SQRT_2;

	static constexpr float SQRT_3 = 1.7320508075688772935274463415059f;
	static constexpr float HALF_SQRT_3 = SQRT_3 / 2.0f;
	static constexpr float INVERSE_SQRT_3 = 1.0f / SQRT_3;

	template<typename T>
	static constexpr T DegreeToRadian(float degree) { return degree * DEGREE_TO_RADIAN; }

	template<typename T>
	static constexpr T RadianToDegree(float radian) { return radian * RADIAN_TO_DEGREE; }

	template<typename T>
	static constexpr T GetEpsilon()
	{
		if constexpr (std::is_same<float, T>())
		{
			return FLOAT_EPSILON;
		}
		else if constexpr (std::is_same<double, T>())
		{
			return DOUBLE_EPSILON;
		}
		else
		{
			return static_cast<T>(0);
		}
	}

	template<typename T>
	static constexpr bool IsEqualTo(T a, T b)
	{
		if constexpr (std::is_same<float, T>())
		{
			return std::fabs(a - b) <= FLOAT_EPSILON;
		}
		else if constexpr (std::is_same<double, T>())
		{
			return std::abs(a - b) <= DOUBLE_EPSILON;
		}
		else
		{
			return a == b;
		}
	}

	template<typename T>
	static constexpr bool IsEqualToZero(T a) { return Math::IsEqualTo(a, static_cast<T>(0)); }

	template<typename T>
	static constexpr bool IsEqualToOne(T a) { return Math::IsEqualTo(a, static_cast<T>(1)); }

	template<typename T>
	static constexpr bool IsSmallThan(T a, T b)
	{
		if constexpr (std::is_same<float, T>())
		{
			return a + FLOAT_EPSILON < b;
		}
		else if constexpr (std::is_same<double, T>())
		{
			return a + DOUBLE_EPSILON < b;
		}
		else
		{
			return a < b;
		}
	}

	template<typename T>
	static constexpr bool IsSmallThanZero(T a) { return Math::IsSmallThan(a, static_cast<T>(0)); }

	template<typename T>
	static constexpr bool IsSmallThanOne(T a) { return Math::IsSmallThan(a, static_cast<T>(1)); }

	template<typename T>
	static constexpr bool IsLargeThan(T a, T b)
	{
		if constexpr (std::is_same<float, T>())
		{
			return a > b + FLOAT_EPSILON;
		}
		else if constexpr (std::is_same<double, T>())
		{
			return a > b + DOUBLE_EPSILON;
		}
		else
		{
			return a > b;
		}
	}

	template<typename T>
	static constexpr bool IsLargeThanZero(T a) { return Math::IsLargeThan(a, static_cast<T>(0)); }

	template<typename T>
	static constexpr bool IsLargeThanOne(T a) { return Math::IsLargeThan(a, static_cast<T>(1)); }

	/// Convert oldValue in [oldMin, oldMax] to newValue in [newMin, newMax].
	template<typename T>
	static constexpr T GetValueInNewRange(T oldValue, T oldMin, T oldMax, T newMin, T newMax)
	{
		return newMin + (oldValue - oldMin) * (newMax - newMin) / (oldMax - oldMin);
	}

	// To save costs on if-branching in math calculations.
	template<typename T>
	static constexpr T FloatSelect(T comparand, T a, T b)
	{
		return comparand >= static_cast<T>(0) ? a : b;
	}
};
}