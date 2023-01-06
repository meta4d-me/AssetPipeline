#pragma once

#include "Base/Template.h"
#include "Constants.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace cd
{

/// <summary>
/// Math vector
/// </summary>
/// <typeparam name="T"> The numeric type : bool, int, float, double, ... </typeparam>
/// <typeparam name="N"> The length of vector : 1, 2, 3, 4, ... </typeparam>
template<typename T, std::size_t N>
class TVector final
{
public:
	using ValueType = T;
	using TVectorN = TVector<T, N>;
	using TVector3 = TVector<T, 3>;
	static constexpr std::size_t Size = N;

public:
	static TVectorN Zero() { return TVectorN(static_cast<T>(0)); }
	static TVectorN One() { return TVectorN(static_cast<T>(1)); }

public:
	// Default uninitialized.
	constexpr TVector() = default;

	// Single value constructor is used to initialize all components to the same value.
	explicit constexpr TVector(T value) { std::fill(Begin(), End(), value); }

	// N parameters constructor.
	template <typename... Args>
	explicit constexpr TVector(Args... args) :
		data { static_cast<T>(args)... }
	{
		static_assert(sizeof...(Args) == N);
	}

	TVector(const TVector&) = default;
	TVector& operator=(const TVector&) = default;
	TVector(TVector&&) = default;
	TVector& operator=(TVector&&) = default;
	~TVector() = default;

	// STL style's iterators.
	using iterator = T*;
	using const_iterator = const T*;
	iterator Begin() { return &data[0]; }
	iterator End() { return &data[0] + Size; }
	const_iterator Begin() const { return &data[0]; }
	const_iterator End() const { return &data[0] + Size; }

	// N parameters setter.
	template <typename... Args>
	void Set(Args... args)
	{
		static_assert(sizeof...(Args) == N);
		data = { static_cast<T>(args)... };
	}

	// Validation.
	CD_FORCEINLINE bool IsZero() const { return static_cast<T>(0) == x() && static_cast<T>(0) == y() && static_cast<T>(0) == z(); }
	CD_FORCEINLINE bool IsNearlyZero(float eps = SmallNumberTolerance) const { return std::abs(x()) <= eps && std::abs(y()) <= eps && std::abs(z()) <= eps; }
	
	CD_FORCEINLINE constexpr T& operator[](int index) { return data[index]; }
	CD_FORCEINLINE constexpr T& operator()(int index) { return data[index]; }
	CD_FORCEINLINE constexpr const T& operator[](int index) const { return data[index]; }
	CD_FORCEINLINE constexpr const T& operator()(int index) const { return data[index]; }

	// Named getters for convenience.
	// You can follow this pattern to write more.
	// It should only generate codes for the real used one.
	CD_FORCEINLINE constexpr T& x()
	{
		static_assert(1 <= N);
		return data[0];
	}

	CD_FORCEINLINE constexpr const T& x() const
	{
		static_assert(1 <= N);
		return data[0];
	}

	CD_FORCEINLINE constexpr T& y()
	{
		static_assert(2 <= N);
		return data[1];
	}

	CD_FORCEINLINE constexpr const T& y() const
	{
		static_assert(2 <= N);
		return data[1];
	}

	CD_FORCEINLINE constexpr T& z()
	{
		static_assert(3 <= N);
		return data[2];
	}

	CD_FORCEINLINE constexpr const T& z() const
	{
		static_assert(3 <= N);
		return data[2];
	}

	CD_FORCEINLINE constexpr T& w()
	{
		static_assert(4 <= N);
		return data[3];
	}

	CD_FORCEINLINE constexpr const T& w() const
	{
		static_assert(4 <= N);
		return data[3];
	}

	// 3D Vector Math
	CD_FORCEINLINE constexpr TVector3 xxx() const
	{
		static_assert(3 <= N);
		return TVector3(x());
	}

	CD_FORCEINLINE constexpr TVector3 yyy() const
	{
		static_assert(3 <= N);
		return TVector3(y());
	}

	CD_FORCEINLINE constexpr TVector3 zzz() const
	{
		static_assert(3 <= N);
		return TVector3(z());
	}

	CD_FORCEINLINE constexpr TVector3 xyz() const
	{
		static_assert(3 <= N);
		return TVector3(x(), y(), z());
	}

	// Math operations
	CD_FORCEINLINE T Length() const { return std::sqrt(LengthSquare()); }
	T LengthSquare() const
	{
		T result{};
		std::for_each(Begin(), End(), [&result](const T& component) { result += component * component; });
		return result;
	}

	TVector& Normalize()
	{
		T length = Length();
		std::for_each(Begin(), End(), [&length](T& component) { component /= length; });
		return *this;
	}

	CD_FORCEINLINE TVector3 Dot(const TVector& rhs) const
	{
		static_assert(3 == N, "Cross products only make sense for 3D vectors!");
		static_assert(std::is_arithmetic_v<T>, "Cross products only make sense for numeric types!");
		return TVector3(x() * rhs.x(), y() * rhs.y(), z() * rhs.z());
	}

	CD_FORCEINLINE TVector3 Cross(const TVector3& rhs) const
	{
		static_assert(3 == N, "Cross products only make sense for 3D vectors!");
		static_assert(std::is_arithmetic_v<T>, "Cross products only make sense for numeric types!");
		return TVector3(
			y() * rhs.z() - z() * rhs.y(),
			z() * rhs.x() - x() * rhs.z(),
			x() * rhs.y() - y() * rhs.x());
	}

	// Mathematics
	TVector& Add(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component += value; });
		return *this;
	}

	TVector& Add(const TVector& other)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&other, &count](T& component) { component += other[count++]; });
		return *this;
	}

	TVector& Minus(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component -= value; });
		return *this;
	}

	TVector& Minus(const TVector& other)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&other, &count](T& component) { component -= other[count++]; });
		return *this;
	}

	TVector& Multiply(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component *= value; });
		return *this;
	}

	TVector& Multiply(const TVector& other)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&other, &count](T& component) { component *= other[count++]; });
		return *this;
	}

	TVector& Divide(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component /= value; });
		return *this;
	}

	TVector& Divide(const TVector& other)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&other, &count](T& component) { component /= other[count++]; });
		return *this;
	}

	CD_FORCEINLINE bool operator!=(const TVector& other) const { return !this == other; }
	bool operator==(const TVector& other) const
	{
		for (int index = 0; index < Size; ++index)
		{
			if constexpr (std::is_floating_point<T>())
			{
				if (std::abs(data[index] - other[index]) > SmallNumberTolerance)
				{
					return false;
				}
			}
			else
			{
				if (data[index] != other[index])
				{
					return false;
				}
			}
		}

		return true;
	}

	// Add
	CD_FORCEINLINE TVector operator+(const TVector& other) const { return TVector(*this).Add(other); }
	CD_FORCEINLINE TVector& operator+=(const TVector& other) { return Add(other); }

	// Minus
	CD_FORCEINLINE TVector operator-() const { return TVector(*this).Multiply(-1); }
	CD_FORCEINLINE TVector operator-(const TVector& other) const { return TVector(*this).Minus(other); }
	CD_FORCEINLINE TVector& operator-=(const TVector& other) { return Minus(other); }

	// Multiply
	CD_FORCEINLINE TVector operator*(T value) const { return TVector(*this).Multiply(value); }
	CD_FORCEINLINE TVector operator*(const TVector& other) const { return TVector(*this).Multiply(other); }
	CD_FORCEINLINE TVector& operator*=(T value) const { return Multiply(value); }
	CD_FORCEINLINE TVector& operator*=(const TVector& other) { return Multiply(other); }

	// Divide
	CD_FORCEINLINE TVector operator/(T value) const { return TVector(*this).Divide(value); }
	CD_FORCEINLINE TVector operator/(const TVector& other) const { return TVector(*this).Divide(other); }
	CD_FORCEINLINE TVector& operator/=(T value) const{ return Divide(value); }
	CD_FORCEINLINE TVector& operator/=(const TVector& other) { return Divide(other); }

private:
	// Validations about template type parameters.
	static_assert(N > 1);
	T data[N];
};

using Vec2f = TVector<float, 2>;
using Vec3f = TVector<float, 3>;
using Vec4f = TVector<float, 4>;
//using Vec2 = TVector<double, 2>;
//using Vec3 = TVector<double, 3>;
//using Vec4 = TVector<double, 4>;
using Point = Vec3f;
using Direction = Vec3f;
using Color = Vec4f;
using UV = Vec2f;

static_assert(sizeof(Vec3f) == 3 * sizeof(float));
static_assert(std::is_standard_layout_v<Vec3f> && std::is_trivial_v<Vec3f>);

}