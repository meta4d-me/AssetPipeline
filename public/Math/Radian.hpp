#pragma once

#include "Degree.hpp"
#include "Math.hpp"

namespace cd
{

template<typename T>
class TRadian
{
public:
	TRadian() = default;
	explicit TRadian(T radian) : m_radian(radian) {}
	explicit TRadian(const TDegree<T>& degree) : m_radian(cd::DegreeToRadian<T>(degree)) {}
	TRadian& operator=(const TDegree<T>& degree) { m_radian = cd::DegreeToRadian<T>(degree); return *this; }
	TRadian(const TRadian&) = default;
	TRadian& operator(const TRadian&) = default;
	TRadian(TRadian&&) = default;
	TRadian& operator(TRadian&&) = default;
	~TRadian() = default;

	T Data() const { return m_radian; }
	TDegree<T> ToDegree() const { return TDegree<T>(m_radian); }

	// Operators
	CD_FORCEINLINE TRadian<T> operator+() const { return *this; }
	CD_FORCEINLINE TRadian<T> operator+(const TRadian& rhs) const { return TRadian<T>(m_radian + rhs.m_radian); }
	CD_FORCEINLINE TRadian<T>& operator+=(const TRadian& rhs) const { m_radian += rhs.m_radian; return *this; }

	CD_FORCEINLINE TRadian<T> operator-() const { return TRadian<T>(-m_radian); }
	CD_FORCEINLINE TRadian<T> operator-(const TRadian& rhs) const { return TRadian<T>(m_radian - rhs.m_radian); }
	CD_FORCEINLINE TRadian<T>& operator-=(const TRadian& rhs) const { m_radian -= rhs.m_radian; return *this; }

	CD_FORCEINLINE TRadian<T> operator*(T scalar) const { return TRadian<T>(m_radian * scalar); }
	CD_FORCEINLINE TRadian<T> operator*=(T scalar) const { m_radian *= scalar; return *this; }

	CD_FORCEINLINE TRadian<T> operator/(T scalar) const { return TRadian<T>(m_radian / scalar); }
	CD_FORCEINLINE TRadian<T> operator/=(T scalar) const { m_radian /= scalar; return *this; }

	CD_FORCEINLINE bool operator<(const TRadian& rhs) const { return m_radian < rhs.m_radian; }
	CD_FORCEINLINE bool operator<=(const TRadian& rhs) const { return m_radian <= rhs.m_radian; }
	CD_FORCEINLINE bool operator>=(const TRadian& rhs) const { return !(*this < rhs); }
	CD_FORCEINLINE bool operator>(const TRadian& rhs) const { return !(*this <= rhs); }
	CD_FORCEINLINE bool operator==(const TRadian& rhs) const { return m_radian == rhs.m_radian; }
	CD_FORCEINLINE bool operator!=(const TRadian& rhs) const { return !(*this == rhs); }

private:
	T m_radian;
};

using Radian = TRadian<float>;

static_assert(sizeof(float) == sizeof(Radian));

}