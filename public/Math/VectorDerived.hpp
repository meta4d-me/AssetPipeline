#pragma once

#include "VectorBase.hpp"

namespace cdtools
{

// Define explicit vector types to create vector instances.
// So that we can specialize behaviors for different vector types.
// For example, Point is (0, 0, 0, 1) and Direction is (0, 0, 0, 0) by default.
enum class VectorType
{
	Point,
	Direction,
	Color,
	UV,
};

/// <summary>
/// CRTP derived class to write common methods about VectorType.
/// </summary>
/// <typeparam name="T"> The numeric type : bool, int, float, double, ... </typeparam>
/// <typeparam name="N"> The length of vector : 1, 2, 3, 4, ... </typeparam>
/// <typeparam name="Vty"> The type of vector usage : Point, Direction, Color, ... </typeparam>
template<typename T, std::size_t N, VectorType Vty>
class VectorDerived : public VectorBase<T, VectorDerived<T, N, Vty>>
{
public:
	using Drived = VectorDerived<T, N, Vty>;
	using Base = VectorBase<T, Drived>;
	using Base::Base;

public:
	void Set(T inX = DefaultValueX(), T inY = DefaultValueY(), T inZ = DefaultValueZ(), T inW = DefaultValueW())
	{
		data[0] = inX;
		data[1] = inY;

		if constexpr(N >= 3)
		{
			data[2] = inZ;
		}
		
		if constexpr(N >= 4)
		{
			data[2] = inZ;
			data[3] = inW;
		}
	}

private:
	static constexpr T DefaultValueX() { return static_cast<T>(0); }

	static constexpr T DefaultValueY()
	{
		if constexpr (VectorType::UV == Vty)
		{
			return static_cast<T>(1);
		}
		else
		{
			return static_cast<T>(0);
		}
	}

	// unsafe
	static constexpr T DefaultValueZ() { return static_cast<T>(0); }

	static constexpr T DefaultValueW()
	{
		if constexpr (VectorType::Direction == Vty)
		{
			return static_cast<T>(0);
		}
		else
		{
			return static_cast<T>(1);
		}
	}

private:
	// By default, base class can't access private or protected members.
	// In CRTP design pattern, we use friend class to remove this limitation.
	friend class VectorBase<T, Drived>;

private:
	// Validations about template type parameters.
	static_assert(N > 1);
	T data[N];
};

using Point = VectorDerived<float, 3, VectorType::Point>;
using Direction = VectorDerived<float, 3, VectorType::Direction>;
using Color = VectorDerived<float, 4, VectorType::Color>;
using U8Color = VectorDerived<uint8_t, 4, VectorType::Color>;
using UV = VectorDerived<float, 2, VectorType::UV>;

}