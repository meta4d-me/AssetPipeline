#pragma once

#include "VectorBase.hpp"

namespace cdtools
{

// Define explicit vector types to create vector instances.
// So that we can specialize behaviors for different vector types.
// For example, Point is (0, 0, 0, 1) and Direction is (0, 0, 0, 0) by default.
enum class VectorType : uint8_t
{
	Generic = 0,
	Point,
	Direction,
	Color,
	U8Color,
	UV,
};

/// <summary>
/// CRTP derived class to write common methods about VectorType.
/// </summary>
/// <typeparam name="T"> The numeric type : bool, int, float, double, ... </typeparam>
/// <typeparam name="N"> The length of vector : 1, 2, 3, 4, ... </typeparam>
/// <typeparam name="Vty"> The type of vector usage : Point, Direction, Color, ... </typeparam>
template<typename T, std::size_t N, VectorType Vty>
class VectorDerived final : public VectorBase<T, VectorDerived<T, N, Vty>>
{
public:
	using Derived = VectorDerived<T, N, Vty>;
	using Base = VectorBase<T, Derived>;
	using Base::Base;

	// Default zero initialization constructor.
	explicit constexpr VectorDerived() :
		data {}
	{
	}

	// Single value constructor is used to initialize all components to the same value.
	explicit constexpr VectorDerived(T value)
	{
		std::fill(this->begin(), this->end(), value);
	}

	// N parameters constructor.
	template <typename... Args>
	explicit constexpr VectorDerived(Args... args) :
		data { static_cast<T>(args)... }
	{
		static_assert(sizeof...(Args) == N);
	}

	// N parameters setter.
	template <typename... Args>
	void Set(Args... args)
	{
		static_assert(sizeof...(Args) == N);
		data = { static_cast<T>(args)... };
	}

	// Named getters for convenience.
	// You can follow this pattern to write more.
	// It should only generate codes for the real used one.
	constexpr T& x()
	{
		static_assert(1 <= N);
		return data[0];
	}

	constexpr const T& x() const
	{
		static_assert(1 <= N);
		return data[0];
	}

	constexpr T& y()
	{
		static_assert(2 <= N);
		return data[1];
	}

	constexpr const T& y() const
	{
		static_assert(2 <= N);
		return data[1];
	}

	constexpr T& z()
	{
		static_assert(3 <= N);
		return data[2];
	}

	constexpr const T& z() const
	{
		static_assert(3 <= N);
		return data[2];
	}

	constexpr T& w()
	{
		static_assert(4 <= N);
		return data[3];
	}

	constexpr const T& w() const
	{
		static_assert(4 <= N);
		return data[3];
	}

	constexpr VectorDerived<T, 3, Vty> xyz() const
	{
		static_assert(3 <= N);
		return VectorDerived<T, 3, Vty>(x(), y(), z());
	}

private:
	// By default, base class can't access private or protected members.
	// In CRTP design pattern, we use friend class to remove this limitation.
	friend class VectorBase<T, Derived>;

private:
	// Validations about template type parameters.
	static_assert(N > 1);
	T data[N];
};

// Generic vector types.
using Vec2f = VectorDerived<float, 2, VectorType::Generic>;
using Vec3f = VectorDerived<float, 3, VectorType::Generic>;
using Vec4f = VectorDerived<float, 4, VectorType::Generic>;
using Vec2 = VectorDerived<double, 2, VectorType::Generic>;
using Vec3 = VectorDerived<double, 3, VectorType::Generic>;
using Vec4 = VectorDerived<double, 4, VectorType::Generic>;

// More safe specific vector types than using generic types.
using Point = VectorDerived<float, 3, VectorType::Point>;
using Direction = VectorDerived<float, 3, VectorType::Direction>;
using Color = VectorDerived<float, 4, VectorType::Color>;
using U8Color = VectorDerived<uint8_t, 4, VectorType::U8Color>;
using UV = VectorDerived<float, 2, VectorType::UV>;

}