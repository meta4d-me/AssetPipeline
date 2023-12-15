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
	static constexpr VectorType Zero() { return VectorType(static_cast<T>(0)); }
	static constexpr VectorType One() { return VectorType(static_cast<T>(1)); }
	static constexpr VectorType Nan() { return VectorType(cd::Math::FLOAT_NAN); }

	static VectorType Lerp(const VectorType& a, const VectorType& b, T factor)
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
	explicit constexpr TVector(T value) { std::fill(begin(), end(), value); }

	// N parameters constructor.
	template <typename... Args>
	explicit constexpr TVector(Args... args) :
		m_data { static_cast<T>(args)... }
	{
		static_assert(sizeof...(Args) == N);
	}

	TVector(const TVector&) = default;
	TVector& operator=(const TVector&) = default;
	TVector(TVector&&) = default;
	TVector& operator=(TVector&&) = default;
	~TVector() = default;

	// Set
	void Set(T value) { std::fill(begin(), end(), value); }

	template <typename... Args>
	void Set(Args... args)
	{
		static_assert(sizeof...(Args) == N);
		m_data = { static_cast<T>(args)... };
	}

	void Clear() { std::memset(m_data, 0, Size * sizeof(float)); }

	// Get
	CD_FORCEINLINE Iterator begin() { return &m_data[0]; }
	CD_FORCEINLINE Iterator end() { return &m_data[0] + Size; }
	CD_FORCEINLINE ConstIterator begin() const { return &m_data[0]; }
	CD_FORCEINLINE ConstIterator end() const { return &m_data[0] + Size; }
	CD_FORCEINLINE constexpr T& operator[](std::size_t index) { return m_data[index]; }
	CD_FORCEINLINE constexpr const T& operator[](std::size_t index) const { return m_data[index]; }
	CD_FORCEINLINE constexpr T& x() { static_assert(1 <= N); return m_data[0]; }
	CD_FORCEINLINE constexpr const T& x() const { static_assert(1 <= N); return m_data[0]; }
	CD_FORCEINLINE constexpr T& y() { static_assert(2 <= N); return m_data[1]; }
	CD_FORCEINLINE constexpr const T& y() const { static_assert(2 <= N); return m_data[1]; }
	CD_FORCEINLINE constexpr T& z() { static_assert(3 <= N); return m_data[2]; }
	CD_FORCEINLINE constexpr const T& z() const { static_assert(3 <= N); return m_data[2]; }
	CD_FORCEINLINE constexpr T& w() { static_assert(4 <= N); return m_data[3]; }
	CD_FORCEINLINE constexpr const T& w() const { static_assert(4 <= N); return m_data[3]; }
	CD_FORCEINLINE constexpr TVector<T, 3> xxx() const { static_assert(3 <= N); return TVector<T, 3>(x()); }
	CD_FORCEINLINE constexpr TVector<T, 3> yyy() const { static_assert(3 <= N); return TVector<T, 3>(y()); }
	CD_FORCEINLINE constexpr TVector<T, 3> zzz() const { static_assert(3 <= N); return TVector<T, 3>(z()); }
	CD_FORCEINLINE constexpr TVector<T, 3> xyz() const { static_assert(3 <= N); return TVector<T, 3>(x(), y(), z()); }

	// Validation
	CD_FORCEINLINE bool IsValid() const
	{
		for (size_t index = 0; index < N; ++index)
		{
			if (!cd::Math::IsValid(m_data[index]))
			{
				return false;
			}
		}

		return true;
	}

	CD_FORCEINLINE bool SameWith(T value) const
	{
		for (size_t index = 0; index < N; ++index)
		{
			if (!Math::IsEqualTo(m_data[index], value))
			{
				return false;
			}
		}

		return true;
	}

	CD_FORCEINLINE bool ContainsNan() const
	{
		for (size_t index = 0; index < N; ++index)
		{
			if (std::isnan(m_data[index]))
			{
				return true;
			}
		}

		return false;
	}

	CD_FORCEINLINE bool Contains(T value) const
	{
		for (size_t index = 0; index < N; ++index)
		{
			if (Math::IsEqualTo(m_data[index], value))
			{
				return true;
			}
		}

		return false;
	}

	// Calculation
	CD_FORCEINLINE T Sum() const
	{
		T result = static_cast<T>(0);
		std::for_each(begin(), end(), [&result](const T& component) { result += component; });
		return result;
	}

	CD_FORCEINLINE T Length() const { return std::sqrt(LengthSquare()); }
	T LengthSquare() const
	{
		T result = static_cast<T>(0);
		std::for_each(begin(), end(), [&result](const T& component) { result += component * component; });
		return result;
	}

	TVector& Normalize()
	{
		T length = Length();
		if (cd::Math::IsEqualToZero(length))
		{
			// avoid devide zero nan.
			return *this;
		}

		T invLength = static_cast<T>(1) / length;
		std::for_each(begin(), end(), [&invLength](T& component) { component *= invLength; });
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
	CD_FORCEINLINE bool operator!=(const TVector& rhs) const { return !(*this == rhs); }
	bool operator==(const TVector& rhs) const
	{
		for (int index = 0; index < Size; ++index)
		{
			if constexpr (std::is_floating_point<T>())
			{
				if (!cd::Math::IsEqualTo(m_data[index], rhs[index]))
				{
					return false;
				}
			}
			else
			{
				if (m_data[index] != rhs[index])
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
		std::for_each(begin(), end(), [&value](T& component) { component += value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator+(const TVector& rhs) const { return TVector(*this) += rhs; }
	TVector& operator+=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(begin(), end(), [&rhs, &count](T& component) { component += rhs[count++]; });
		return *this;
	}

	CD_FORCEINLINE TVector operator-(T value) const { return TVector(*this) -= value; }
	TVector& operator-=(T value)
	{
		std::for_each(begin(), end(), [&value](T& component) { component -= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator-(const TVector& rhs) const { return TVector(*this) -= rhs; }
	TVector& operator-=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(begin(), end(), [&rhs, &count](T& component) { component -= rhs[count++]; });
		return *this;
	}

	friend CD_FORCEINLINE TVector operator*(T lhs, const TVector& rhs) { return rhs * lhs; }
	CD_FORCEINLINE TVector operator*(T value) const { return TVector(*this) *= value; }
	TVector& operator*=(T value)
	{
		std::for_each(begin(), end(), [&value](T& component) { component *= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator*(const TVector& rhs) const { return TVector(*this) *= rhs; }
	TVector& operator*=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(begin(), end(), [&rhs, &count](T& component) { component *= rhs[count++]; });
		return *this;
	}

	CD_FORCEINLINE TVector operator/(T value) const { return TVector(*this) /= value; }
	TVector& operator/=(T value)
	{
		std::for_each(begin(), end(), [&value](T& component) { component /= value; });
		return *this;
	}
	CD_FORCEINLINE TVector operator/(const TVector& rhs) const { return TVector(*this) /= rhs; }
	TVector& operator/=(const TVector& rhs)
	{
		int count = 0;
		std::for_each(begin(), end(), [&rhs, &count](T& component) { component /= rhs[count++]; });
		return *this;
	}

private:
	// Validations about template type parameters.
	static_assert(N > 1);
	T m_data[N];
};

using Vec2f = TVector<float, 2>;
using Vec3f = TVector<float, 3>;
using Vec4f = TVector<float, 4>;

static_assert(2 * sizeof(float) == sizeof(Vec2f));
static_assert(3 * sizeof(float) == sizeof(Vec3f));
static_assert(4 * sizeof(float) == sizeof(Vec4f));
//static_assert(std::is_standard_layout_v<Vec2f> && std::is_trivial_v<Vec2f>);
//static_assert(std::is_standard_layout_v<Vec3f> && std::is_trivial_v<Vec3f>);
//static_assert(std::is_standard_layout_v<Vec4f> && std::is_trivial_v<Vec4f>);

}