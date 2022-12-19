#pragma once

#include <stdint.h>

namespace cdtools
{

template<typename T, std::size_t N>
constexpr T array_sum(T const (&array)[N])
{
	T sum = 0;
	for (std::size_t i = 0; i < N; ++i)
	{
		sum += array[i];
	}

	return sum;
};

constexpr float ESP_VERTEX_DATA = 0.00000001f;
static bool IsEqual(float a, float b)
{
	float delta = a - b;
	return delta < ESP_VERTEX_DATA && delta > -ESP_VERTEX_DATA;
}

template<typename T>
constexpr T lerp(const T& a, const T& b, const float t)
{
	static_assert(std::is_arithmetic<T>::value, "Input must be a number!");
	return (a * (1 - t) + b * t);
}

template<typename T>
constexpr T clamp(const T& x, const T& a, const T& b)
{
	static_assert(std::is_arithmetic<T>::value, "Input must be a number!");
	return (x < a ? a : (x > b ? b : x));
}

template<typename T>
constexpr T step(const T& a, const T& x)
{
	static_assert(std::is_arithmetic<T>::value, "Input must be a number!");
	return x >= a ? static_cast<T>(1) : static_cast<T>(0);
}

template<typename T>
constexpr T pulse(const T& a, const T& b, const T& x)
{
	static_assert(std::is_arithmetic<T>::value, "Input must be a number!");
	return (step<T>(a, x) - step<T>(b, x));
}

template<typename T>
constexpr T smoothstep(const T& a, const T& b, const T& x)
{
	static_assert(std::is_floating_point<T>::value, "Input must be a floating point!");
	if (x < a)
	{
		return static_cast<T>(0);
	}
	if (x >= b)
	{
		return static_cast<T>(1);
	}
	T f = (x - a) / (b - a);	//Calculate the ratio
	return (f*f * (3 - 2*f));	//3x^2 - 2x^3
}

template<typename T>
constexpr T smoothstep_high(const T& a, const T& b, const T& x)
{
	static_assert(std::is_floating_point<T>::value, "Input must be a floating point!");
	if (x < a)
	{
		return static_cast<T>(0);
	}
	if (x >= b)
	{
		return static_cast<T>(1);
	}
	T f = (x - a) / (b - a);				//Calculate the ratio
	return (f*f*f * (6*f*f - 15*f + 10));	//6x^5-15x^4+10x^3
}

template<typename T, typename IntType>
constexpr T pmod(T a, T b)
{
	static_assert(std::is_floating_point<T>::value, "Input must be a floating point!");
	IntType n = static_cast<IntType>(a / b);
	a -= n*b;
	if (a < 0)
	{
		a += b;
	}
	return a;
}

}