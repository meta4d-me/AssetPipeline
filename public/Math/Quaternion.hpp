#pragma once

#include "Math/Vector.hpp"
#include "Math/Vector.hpp"

namespace cd
{

template<typename T>
class TQuaternion
{
public:
	using Vec = TVector<T, 3>;

	static TQuaternion<T> SphericalLerp(const TQuaternion<T>& a, const TQuaternion<T>& b, T s)
	{

	}

public:
	TQuaternion() = default;
	explicit TQuaternion(T s, T vx, T vy, T vz) : m_scalar(s), m_vector(vx, vy, vz) {}
	explicit TQuaternion(T s, Vec v) : m_scalar(s), m_vector(cd::MoveTemp(v)) {}
	TQuaternion(const TQuaternion&) = default;
	TQuaternion& operator=(const TQuaternion&) = default;
	TQuaternion(TQuaternion&&) = default;
	TQuaternion& operator=(TQuaternion&&) = default;
	~TQuaternion() = default;

	CD_FORCEINLINE T GetScalar() const { return m_scalar; }
	CD_FORCEINLINE void SetScalar(T s) { m_scalar = s; }

	CD_FORCEINLINE const T& GetVector() const { return m_vector; }
	CD_FORCEINLINE void SetVector(Vec v) { m_vector = cd::MoveTemp(v); }
	CD_FORCEINLINE T x() const { return m_vector.x(); }
	CD_FORCEINLINE T y() const { return m_vector.y(); }
	CD_FORCEINLINE T z() const { return m_vector.z(); }

	// Validations
	CD_FORCEINLINE bool isNaN() const { return std::isnan(m_scalar) || std::isnan(m_vector.x()) || std::isnan(m_vector.y()) || std::isnan(m_vector.z()); }

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
	CD_FORCEINLINE TQuaternion<T>& operator+() const { return *this; }
	CD_FORCEINLINE TQuaternion<T> operator+(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar + rhs.m_scalar, m_vector + rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator+=(const TQuaternion<T>& rhs) { m_scalar += rhs.m_scalar; m_vector += rhs.m_vector; return *this; }
	
	CD_FORCEINLINE TQuaternion<T> operator-() const { return Quaternion(-m_scalar, -m_vector.x(), -m_vector.y(), -m_vector.z()); }
	CD_FORCEINLINE TQuaternion<T> operator-(const TQuaternion<T>& rhs) const { return TQuaternion<T>(m_scalar - rhs.m_scalar, m_vector - rhs.m_vector); }
	CD_FORCEINLINE TQuaternion<T>& operator-=(const TQuaternion<T>& rhs) { m_scalar -= rhs.m_scalar; m_vector -= rhs.m_vector; return *this; }

	CD_FORCEINLINE TQuaternion<T> operator*(T scalar) const { return TQuaternion<T>(m_scalar * scalar, m_vector.x() * scalar, m_vector.y() * scalar, m_vector.z() * scalar); }
	CD_FORCEINLINE TQuaternion<T> operator*(const TQuaternion<T>& rhs) const {
		return TQuaternion<T>(m_scalar * rhs.m_scalar - m_vector.Dot(rhs.m_vector),
			m_scalar * rhs.m_vector + rhs.m_scalar * m_vector + m_vector.Cross(rhs.m_vector));
	}

	CD_FORCEINLINE TQuaternion<T> operator/(T scalar) const { return TQuaternion<T>(m_scalar / scalar, m_vector.x() / scalar, m_vector.y() / scalar, m_vector.z() / scalar); }

	CD_FORCEINLINE bool operator==(const TQuaternion& rhs) const { return m_scalar == rhs.m_scalar && m_vector == rhs.m_vector; }
	CD_FORCEINLINE bool operator!=(const TQuaternion& rhs) const { return !(*this == rhs); }

private:
	T m_scalar;
	Vec m_vector;
};
	
using Quaternion = TQuaternion<float>;

static_assert(4 * sizeof(float) == sizeof(Quaternion));
static_assert(std::is_standard_layout_v<Quaternion> && std::is_trivial_v<Quaternion>);

}