#pragma once

#include "Base/Template.h"
#include "Math/AxisSystem.hpp"
#include "Math/Math.hpp"

#include <cstring> // std::memset

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
	using VectorType = TVector<T, N>;
	static constexpr std::size_t Size = N;
	using Iterator = T*;
	using ConstIterator = const T*;

public:
	static VectorType Zero() { return VectorType(static_cast<T>(0)); }
	static VectorType One() { return VectorType(static_cast<T>(1)); }

	static VectorType Lerp(const VectorType& a, const VectorType& b, float factor)
	{
		return a + (b - a) * factor;
	}

	static TVector<T, 3> GetUpAxis(const AxisSystem& axisSystem)
	{
		UpVector up = axisSystem.GetUpVector();
		if (UpVector::XAxis == up)
		{
			return TVector<T, 3>(1.0f, 0.0f, 0.0f);
		}
		else if (UpVector::YAxis == up)
		{
			return TVector<T, 3>(0.0f, 1.0f, 0.0f);
		}
		else
		{
			return TVector<T, 3>(0.0f, 0.0f, 1.0f);
		}
	}

	static TVector<T, 3> GetFrontAxis(const AxisSystem& axisSystem)
	{
		UpVector up = axisSystem.GetUpVector();
		FrontVector front = axisSystem.GetFrontVector();
		if (UpVector::XAxis == up)
		{
			if (FrontVector::ParityEven == front)
			{
				return TVector<T, 3>(0.0f, 1.0f, 0.0f);
			}
			else
			{
				return TVector<T, 3>(0.0f, 0.0f, 1.0f);
			}
		}
		else if (UpVector::YAxis == up)
		{
			if (FrontVector::ParityEven == front)
			{
				return TVector<T, 3>(1.0f, 0.0f, 0.0f);
			}
			else
			{
				return TVector<T, 3>(0.0f, 0.0f, 1.0f);
			}
		}
		else
		{
			if (FrontVector::ParityEven == front)
			{
				return TVector<T, 3>(1.0f, 0.0f, 0.0f);
			}
			else
			{
				return TVector<T, 3>(0.0f, 1.0f, 0.0f);
			}
		}
	}

public:
	// Default uninitialized.
	constexpr TVector() = default;

	// Single value constructor is used to initialize all components to the same value.
	explicit constexpr TVector(T value) { std::fill(Begin(), End(), value); }

	// Smaller size vector + single value constructor.
	//explicit constexpr TVector(TVector<T, Size - 1> vector, T value)
	//{
	//	static_assert(N > 2);
	//	std::fill(Begin(), Begin() + Size - 1, value);
	//	data[Size - 1] = value;
	//}

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

	// Set
	void Set(T value) { std::fill(Begin(), End(), value); }

	template <typename... Args>
	void Set(Args... args)
	{
		static_assert(sizeof...(Args) == N);
		data = { static_cast<T>(args)... };
	}

	void Clear() { std::memset(data, 0, Size * sizeof(float)); }

	// Get
	CD_FORCEINLINE Iterator Begin() { return &data[0]; }
	CD_FORCEINLINE Iterator End() { return &data[0] + Size; }
	CD_FORCEINLINE ConstIterator Begin() const { return &data[0]; }
	CD_FORCEINLINE ConstIterator End() const { return &data[0] + Size; }
	CD_FORCEINLINE constexpr T& operator[](std::size_t index) { return data[index]; }
	CD_FORCEINLINE constexpr const T& operator[](std::size_t index) const { return data[index]; }
	CD_FORCEINLINE constexpr T& x() { static_assert(1 <= N); return data[0]; }
	CD_FORCEINLINE constexpr const T& x() const { static_assert(1 <= N); return data[0]; }
	CD_FORCEINLINE constexpr T& y() { static_assert(2 <= N); return data[1]; }
	CD_FORCEINLINE constexpr const T& y() const { static_assert(2 <= N); return data[1]; }
	CD_FORCEINLINE constexpr T& z() { static_assert(3 <= N); return data[2]; }
	CD_FORCEINLINE constexpr const T& z() const { static_assert(3 <= N); return data[2]; }
	CD_FORCEINLINE constexpr T& w() { static_assert(4 <= N); return data[3]; }
	CD_FORCEINLINE constexpr const T& w() const { static_assert(4 <= N); return data[3]; }
	CD_FORCEINLINE constexpr TVector<T, 3> xxx() const { static_assert(3 <= N); return TVector<T, 3>(x()); }
	CD_FORCEINLINE constexpr TVector<T, 3> yyy() const { static_assert(3 <= N); return TVector<T, 3>(y()); }
	CD_FORCEINLINE constexpr TVector<T, 3> zzz() const { static_assert(3 <= N); return TVector<T, 3>(z()); }
	CD_FORCEINLINE constexpr TVector<T, 3> xyz() const { static_assert(3 <= N); return TVector<T, 3>(x(), y(), z()); }

	// Validation
	CD_FORCEINLINE bool IsNaN() const { return std::isnan(x()) || std::isnan(y()) || std::isnan(z()); }
	CD_FORCEINLINE bool IsZero() const
	{
		if constexpr (2 == N)
		{
			return Math::IsEqualToZero(x()) && Math::IsEqualToZero(y());
		}
		else if constexpr (3 == N)
		{
			return Math::IsEqualToZero(x()) && Math::IsEqualToZero(y()) && Math::IsEqualToZero(z());
		}
		else if constexpr (4 == N)
		{
			return Math::IsEqualToZero(x()) && Math::IsEqualToZero(y()) && Math::IsEqualToZero(z()) && Math::IsEqualToZero(w());
		}
	}

	// Calculation
	CD_FORCEINLINE T Sum() const
	{
		if constexpr (2 == N)
		{
			return x() + y();
		}
		else if constexpr (3 == N)
		{
			return x() + y() + z();
		}
		else if constexpr (4 == N)
		{
			return x() + y() + z() + w();
		}
	}

	CD_FORCEINLINE T Length() const { return std::sqrt(LengthSquare()); }
	T LengthSquare() const
	{
		T result = static_cast<T>(0);
		std::for_each(Begin(), End(), [&result](const T& component) { result += component * component; });
		return result;
	}

	TVector& Normalize()
	{
		T length = Length();
		std::for_each(Begin(), End(), [&length](T& component) { component /= length; });
		return *this;
	}

	CD_FORCEINLINE T Dot(const TVector& rhs) const
	{
		static_assert(N >= 3);
		return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
	}

	CD_FORCEINLINE TVector<T, 3> Cross(const TVector<T, 3>& rhs) const
	{
		static_assert(N >= 3);
		return TVector<T, 3>(y() * rhs.z() - z() * rhs.y(),
							 z() * rhs.x() - x() * rhs.z(),
							 x() * rhs.y() - y() * rhs.x());
	}

	// Operators
	CD_FORCEINLINE bool operator!=(const TVector& rhs) const { return !this == rhs; }
	bool operator==(const TVector& rhs) const
	{
		for (int index = 0; index < Size; ++index)
		{
			if constexpr (std::is_floating_point<T>())
			{
				if (std::abs(data[index] - rhs[index]) > Math::GetEpsilon<T>())
				{
					return false;
				}
			}
			else
			{
				if (data[index] != rhs[index])
				{
					return false;
				}
			}
		}

		return true;
	}

	CD_FORCEINLINE TVector operator+(T value) const { return TVector(*this) += value; }
	TVector& operator+=(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component += value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator+(const TVector& rhs) const { return TVector(*this) += rhs; }
	TVector& operator+=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&rhs, &count](T& component) { component += rhs[count++]; });
		return *this;
	}

	CD_FORCEINLINE TVector operator-(T value) const { return TVector(*this) -= value; }
	TVector& operator-=(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component -= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator-(const TVector& rhs) const { return TVector(*this) -= rhs; }
	TVector& operator-=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&rhs, &count](T& component) { component -= rhs[count++]; });
		return *this;
	}

	CD_FORCEINLINE TVector operator*(T value) const { return TVector(*this) *= value; }
	TVector& operator*=(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component *= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator*(const TVector& rhs) const { return TVector(*this) *= rhs; }
	TVector& operator*=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&rhs, &count](T& component) { component *= rhs[count++]; });
		return *this;
	}

	CD_FORCEINLINE TVector operator/(T value) const { return TVector(*this) /= value; }
	TVector& operator/=(T value)
	{
		std::for_each(Begin(), End(), [&value](T& component) { component /= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator/(const TVector& rhs) const { return TVector(*this) /= rhs; }
	TVector& operator/=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(Begin(), End(), [&rhs, &count](T& component) { component /= rhs[count++]; });
		return *this;
	}

private:
	// Validations about template type parameters.
	static_assert(N > 1);
	T data[N];
};

using Vec2f = TVector<float, 2>;
using Vec3f = TVector<float, 3>;
using Vec4f = TVector<float, 4>;
using Point = Vec3f;
using Direction = Vec3f;
using Color = Vec4f;
using UV = Vec2f;

static_assert(2 * sizeof(float) == sizeof(Vec2f));
static_assert(3 * sizeof(float) == sizeof(Vec3f));
static_assert(4 * sizeof(float) == sizeof(Vec4f));
static_assert(std::is_standard_layout_v<Vec2f> && std::is_trivial_v<Vec2f>);
static_assert(std::is_standard_layout_v<Vec3f> && std::is_trivial_v<Vec3f>);
static_assert(std::is_standard_layout_v<Vec4f> && std::is_trivial_v<Vec4f>);

}