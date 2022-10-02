#pragma once

#include "VectorBase.hpp"

namespace cdtools
{

// Define explicit vector types to create vector instances.
// So that we can specialize behaviors for different vector types.
// For example, Point is (0, 0, 0, 1) and Direction is (0, 0, 0, 0) by default.
enum class VectorType : uint8_t
{
	Point = 0,
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
	explicit VectorDerived()
	{
		if constexpr (4 == N)
		{
			if constexpr (VectorType::Point == Vty)
			{
				data[0] = data[1] = data[2] = static_cast<T>(0);
				data[3] = static_cast<T>(1);
			}
		}
		else if constexpr (2 == N)
		{
			if constexpr (VectorType::UV == Vty)
			{
				data[0] = static_cast<T>(0);
				data[1] = static_cast<T>(1);
			}
		}
		else
		{
			std::fill(std::begin(data), std::end(data), static_cast<T>(0));
		}
	}

	template <typename... Args>
	explicit VectorDerived(Args... args) :
		data{ static_cast<T>(args)... }
	{
		static_assert(sizeof...(Args) == N);
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