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
	return delta < ESP_VERTEX_DATA&& delta > -ESP_VERTEX_DATA;
}

}