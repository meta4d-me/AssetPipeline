#pragma once

#include <algorithm>

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
typename std::enable_if<std::is_arithmetic<T>::value>::type clamp(const T& x, const T& a, const T& b)
{
	return (x < a ? a : (x > b ? b : x));
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value>::type step(const T& a, const T& x)
{
	return x >= a ? static_cast<T>(1) : static_cast<T>(0);
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value>::type pulse(const T& a, const T& b, const T& x)
{
	return (step<T>(a, x) - step<T>(b, x));
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type smoothstep(const T& a, const T& b, const T& x)
{
	if (x < a)
	{
		return static_cast<T>(0);
	}
	if (x >= b)
	{
		return static_cast<T>(1);
	}
	T f = (x - a) / (b - a);	//Calculate the ratio
	return (f*f * (3 - 2.0f));	//3x^2 - 2x^3
}

template<typename T, typename IntType>
typename std::enable_if<std::is_floating_point<T>::value>::type pmod(T a, T b)
{
	IntType n = static_cast<IntType>(a / b);
	a -= n*b;
	if (a < 0)
	{
		a += b;
	}
	return a;
}

}