#pragma once

#include "VectorBase.hpp"

#include <type_traits>

namespace cd
{

/// <summary>
/// CRTP derived class to write common methods.
/// </summary>
/// <typeparam name="T"> The numeric type : bool, int, float, double, ... </typeparam>
/// <typeparam name="N"> The length of vector : 1, 2, 3, 4, ... </typeparam>
template<typename T, std::size_t N>
class VectorDerived final : public VectorBase<T, VectorDerived<T, N>>
{
public:
	using ValueType = T;
	using Derived = VectorDerived<T, N>;
	using Vec3Derived = VectorDerived<T, 3>;
	using Base = VectorBase<T, Derived>;
	using Base::Base;

public:
	static Derived Zero() { return Derived(static_cast<T>(0)); }
	static Derived One() { return Derived(static_cast<T>(1)); }

public:
	// Default zero initialization constructor.
	explicit constexpr VectorDerived() :
		data{}
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

	VectorDerived(const VectorDerived& rhs)
	{
		std::copy(rhs.begin(), rhs.end(), this->begin());
	}

	VectorDerived(VectorDerived&& rhs)
	{
		// Nothing to move since it's primitive array
		std::copy(rhs.begin(), rhs.end(), this->begin());
		// Still reset the other array to mimic a "move"
		std::fill(rhs.begin(), rhs.end(), static_cast<ValueType>(0));
	}

	VectorDerived& operator=(const VectorDerived& rhs)
	{
		std::copy(rhs.begin(), rhs.end(), this->begin());
		return *this;
	}

	VectorDerived& operator=(VectorDerived&& rhs)
	{
		// Nothing to move since it's primitive array
		std::copy(rhs.begin(), rhs.end(), this->begin());
		// Still reset the other array to mimic a "move"
		std::fill(rhs.begin(), rhs.end(), static_cast<ValueType>(0));
		return *this;
	}

	~VectorDerived() = default;

	// N parameters setter.
	template <typename... Args>
	void Set(Args... args)
	{
		static_assert(sizeof...(Args) == N);
		data = { static_cast<T>(args)... };
	}

	// Validation.
	bool IsZero() const { return static_cast<T>(0) == x() && static_cast<T>(0) == y() && static_cast<T>(0) == z(); }
	bool IsNearlyZero(float eps = SmallNumberTolerance) const { return std::abs(x()) <= eps && std::abs(y()) <= eps && std::abs(z()) <= eps; }

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

	// 3D Vector Math
	constexpr Vec3Derived xxx() const
	{
		static_assert(3 <= N);
		return Vec3Derived(x());
	}

	constexpr Vec3Derived yyy() const
	{
		static_assert(3 <= N);
		return Vec3Derived(y());
	}

	constexpr Vec3Derived zzz() const
	{
		static_assert(3 <= N);
		return Vec3Derived(z());
	}

	constexpr Vec3Derived xyz() const
	{
		static_assert(3 <= N);
		return Vec3Derived(x(), y(), z());
	}

	// Math operations
	Vec3Derived Dot(const Vec3Derived& rhs) const
	{
		static_assert(3 == N, "Cross products only make sense for 3D vectors!");
		static_assert(std::is_arithmetic_v<T>, "Cross products only make sense for numeric types!");
		return Vec3Derived(x() * rhs.x(), y() * rhs.y(), z() * rhs.z());
	}

	Vec3Derived Cross(const Vec3Derived& rhs) const
	{
		static_assert(3 == N, "Cross products only make sense for 3D vectors!");
		static_assert(std::is_arithmetic_v<T>, "Cross products only make sense for numeric types!");
		return Vec3Derived(
			y() * rhs.z() - z() * rhs.y(),
			z() * rhs.x() - x() * rhs.z(),
			x() * rhs.y() - y() * rhs.x());
	}

	// Add
	Derived operator+(const Derived& other) const { return Derived(*this).Add(other); }
	Derived& operator+=(const Derived& other) { return Add(other); }

	// Minus
	Derived operator-() const { return Derived(*this).Multiply(-1); }
	Derived operator-(const Derived& other) const { return Derived(*this).Minus(other); }
	Derived& operator-=(const Derived& other) { return Minus(other); }

	// Multiply
	Derived operator*(T value) const { return Derived(*this).Multiply(value); }
	Derived operator*(const Derived& other) const { return Derived(*this).Multiply(other); }
	Derived& operator*=(T value) const { return Derived(*this).Multiply(value); }
	Derived& operator*=(const Derived& other) { return Multiply(other); }

	// Divide
	Derived operator/(T value) const { return Derived(*this).Divide(value); }
	Derived operator/(const Derived& other) const { return Derived(*this).Divide(other); }
	Derived& operator/=(T value) const{ return Derived(*this).Divide(value); }
	Derived& operator/=(const Derived& other) { return Divide(other); }

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
using Vec2f = VectorDerived<float, 2>;
using Vec3f = VectorDerived<float, 3>;
using Vec4f = VectorDerived<float, 4>;
//using Vec2 = VectorDerived<double, 2>;
//using Vec3 = VectorDerived<double, 3>;
//using Vec4 = VectorDerived<double, 4>;

// Removed vector type enum as it is too complex to write relationships between different usages.
// It will be a graph to describe rules about Additivity, Interchangeability, Multipliability, ...
using Point = Vec3f;
using Direction = Vec3f;
using Color = Vec4f;
using UV = Vec2f;

//static_cast(std::is_standard_layout_v<Vec3f> && std::is_trivial_v<Vec3f>, "VectorDerived needs to implement copy/move constructors.");

}