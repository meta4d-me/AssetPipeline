#pragma once

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"

namespace cd
{

template<typename T>
class TQuaternion
{
public:
	using ValueType = T;
	static constexpr std::size_t Size = 4;
	using Iterator = T*;
	using ConstIterator = const T*;

	static TQuaternion<T> FromAxisAngle(const TVector<T, 3>& axis, T angleRadian)
	{
		T halfAngle = angleRadian * static_cast<T>(0.5);
		T sinHalfAngle = std::sin(halfAngle);
		return TQuaternion<T>(std::cos(halfAngle), axis.x() * sinHalfAngle, axis.y() * sinHalfAngle, axis.z() * sinHalfAngle);
	}

	//static TQuaternion<T> SphericalLerp(const TQuaternion<T>& a, const TQuaternion<T>& b, T s)
	//{
	//
	//}

public:
	TQuaternion() = default;
	explicit TQuaternion(T s, T vx, T vy, T vz) : m_scalar(s), m_vector(vx, vy, vz) {}
	explicit TQuaternion(T s, TVector<T, 3> v) : m_scalar(s), m_vector(cd::MoveTemp(v)) {}
	explicit TQuaternion(const TMatrix<T, 3, 3>& rotationMatrix)
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T half = static_cast<T>(0.5);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		// Same to Eigen's implementation based on "Quaternion Calculus and Fast Animation" Ken Shoemake, 1987 SIGGRAPH.
		// TODO : Have a look at intel report https://www.intel.com/content/dam/develop/external/us/en/documents/293748-142817.pdf.
		T t = rotationMatrix.Trace();
		if (t > zero)
		{
			t = std::sqrt(t + one);
			w() = half * t;

			t = half / t;
			x() = rotationMatrix.Data(2, 1) - rotationMatrix.Data(1, 2) * t;
			y() = rotationMatrix.Data(0, 2) - rotationMatrix.Data(2, 0) * t;
			z() = rotationMatrix.Data(1, 0) - rotationMatrix.Data(0, 1) * t;
		}
		else
		{
			int i = 0;
			if (rotationMatrix.Data(1, 1) > rotationMatrix.Data(0, 0))
			{
				i = 1;
			}

			if (rotationMatrix.Data(2, 2) > rotationMatrix.Data(i, i))
			{
				i = 2;
			}
				
			int j = (i + 1) % 3;
			int k = (j + 1) % 3;
			t = std::sqrt(rotationMatrix.Data(i, i) - rotationMatrix.Data(j, j) - rotationMatrix.Data(k, k) + one);
			Data(i) = half * t;
			t = half / t;
			w() = (rotationMatrix.Data(k, j) - rotationMatrix.Data(j, k)) * t;
			Data(j) = (rotationMatrix.Data(j, i) + rotationMatrix.Data(i, j)) * t;
			Data(k) = (rotationMatrix.Data(k, i) + rotationMatrix.Data(i, k)) * t;
		}
	}
	TQuaternion(const TQuaternion&) = default;
	TQuaternion& operator=(const TQuaternion&) = default;
	TQuaternion(TQuaternion&&) = default;
	TQuaternion& operator=(TQuaternion&&) = default;
	~TQuaternion() = default;

	void Clear() { std::memset(Begin(), 0, Size); }

	// Get
	CD_FORCEINLINE Iterator Begin() { return m_vector.Begin(); }
	CD_FORCEINLINE Iterator End() { return &m_scalar + 1; }
	CD_FORCEINLINE ConstIterator Begin() const { return m_vector.Begin(); }
	CD_FORCEINLINE ConstIterator End() const { return &m_scalar + 1; }
	CD_FORCEINLINE T& Data(int index) { return *(Begin() + index); }
	CD_FORCEINLINE T Data(int index) const { return *(Begin() + index); }
	CD_FORCEINLINE T GetScalar() const { return m_scalar; }
	CD_FORCEINLINE void SetScalar(T s) { m_scalar = s; }
	CD_FORCEINLINE const T& GetVector() const { return m_vector; }
	CD_FORCEINLINE void SetVector(TVector<T, 3> v) { m_vector = cd::MoveTemp(v); }
	CD_FORCEINLINE T x() const { return m_vector.x(); }
	CD_FORCEINLINE T y() const { return m_vector.y(); }
	CD_FORCEINLINE T z() const { return m_vector.z(); }
	CD_FORCEINLINE T w() const { return m_scalar; }
	CD_FORCEINLINE T& x() { return m_vector.x(); }
	CD_FORCEINLINE T& y() { return m_vector.y(); }
	CD_FORCEINLINE T& z() { return m_vector.z(); }
	CD_FORCEINLINE T& w() { return m_scalar; }

	// Validations
	CD_FORCEINLINE bool isNaN() const { return std::isnan(m_scalar) || std::isnan(m_vector.x()) || std::isnan(m_vector.y()) || std::isnan(m_vector.z()); }

	// Conversions
	TMatrix<T, 3, 3> ToMatrix3x3() const
	{
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T tx = two * x();
		T ty = two * y();
		T tz = two * z();
		T twx = tx * w();
		T twy = ty * w();
		T twz = tz * w();
		T txx = tx * x();
		T txy = ty * x();
		T txz = tz * x();
		T tyy = ty * y();
		T tyz = tz * y();
		T tzz = tz * z();

		return TMatrix<T, 3, 3>(1 - (tyy + tzz), txy + twz, txz - twy,
								txy - twz, one - (txx + tzz), tyz + twx,
								txz + twy, tyz - twx, one - (txx + tyy));
	}

	// Calculations
	CD_FORCEINLINE TQuaternion<T> Inverse() const { return TQuaternion<T>(m_scalar, -m_vector); }
	CD_FORCEINLINE T Dot(const TQuaternion& rhs) const { return m_scalar * rhs.m_scalar + m_vector.x() * rhs.m_vector.x() + m_vector.y() * rhs.m_vector.y() + m_vector.z() * rhs.m_vector.z(); }
	CD_FORCEINLINE T LengthSqure() const { return m_scalar * m_scalar + m_vector.x() * m_vector.x() + m_vector.y() * m_vector.y() + m_vector.z() * m_vector.z();  }
	CD_FORCEINLINE T Length() const { return std::sqrt(LengthSqure());  }
	TQuaternion<T>& Normalize()
	{
		T factor = static_cast<T>(1) / Length();
		m_scalar *= factor;
		m_vector *= factor;
		return *this;
	}

	// Operators
	CD_FORCEINLINE TQuaternion<T> operator+(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar + rhs.m_scalar, m_vector + rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator+=(const TQuaternion<T>& rhs) { m_scalar += rhs.m_scalar; m_vector += rhs.m_vector; return *this; }
	
	CD_FORCEINLINE TQuaternion<T> operator-(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar - rhs.m_scalar, m_vector - rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator-=(const TQuaternion<T>& rhs) { m_scalar -= rhs.m_scalar; m_vector -= rhs.m_vector; return *this; }

	CD_FORCEINLINE TQuaternion<T> operator*(T scalar) const { return TQuaternion<T>(m_scalar * scalar, m_vector.x() * scalar, m_vector.y() * scalar, m_vector.z() * scalar); }
	CD_FORCEINLINE TQuaternion<T> operator*(const TQuaternion<T>& rhs) const
	{
		return TQuaternion<T>(m_scalar * rhs.m_scalar - m_vector.Dot(rhs.m_vector),
			m_scalar * rhs.m_vector + rhs.m_scalar * m_vector + m_vector.Cross(rhs.m_vector));
	}
	CD_FORCEINLINE TVector<T, 3> operator*(const TVector<T, 3>& v) const
	{
		T doubleScalar = m_scalar + m_scalar;
		return m_vector.Cross(v) * doubleScalar + v * (doubleScalar * m_scalar - static_cast<T>(1)) + m_vector * m_vector.Dot(v) * static_cast<T>(2);
	}

	CD_FORCEINLINE TQuaternion<T> operator/(T scalar) const { return TQuaternion<T>(m_scalar / scalar, m_vector.x() / scalar, m_vector.y() / scalar, m_vector.z() / scalar); }

	CD_FORCEINLINE bool operator==(const TQuaternion& rhs) const { return m_scalar == rhs.m_scalar && m_vector == rhs.m_vector; }
	CD_FORCEINLINE bool operator!=(const TQuaternion& rhs) const { return !(*this == rhs); }

private:
	TVector<T, 3> m_vector;
	T m_scalar;
};
	
using Quaternion = TQuaternion<float>;

static_assert(4 * sizeof(float) == sizeof(Quaternion));
static_assert(std::is_standard_layout_v<Quaternion> && std::is_trivial_v<Quaternion>);

}