#pragma once

#include "Base/Platform.h"
#include "Math/Math.hpp"
#include "Radian.hpp"

namespace cd
{

template<typename T>
class TDegree
{
public:
	TDegree() = default;
	explicit TDegree(T degree) : m_degree(degree) {}
	explicit TDegree(const TRadian<T>& radian) : m_degree(Math::RadianToDegree<T>(radian)) {}
	TDegree& operator=(const TRadian<T>& radian) { m_degree = Math::RadianToDegree<T>(radian); return *this; }
	TDegree(const TDegree&) = default;
	TDegree& operator=(const TDegree&) = default;
	TDegree(TDegree&&) = default;
	TDegree& operator=(TDegree&&) = default;
	~TDegree() = default;

	CD_FORCEINLINE T Data() const { return m_degree; }
	CD_FORCEINLINE TRadian<T> ToRadian() const { return TRadian<T>(m_degree); }

	// Operators
	CD_FORCEINLINE TDegree<T>& operator+() const { return *this; }
	CD_FORCEINLINE TDegree<T> operator+(const TDegree& rhs) const { return TDegree<T>(m_degree + rhs.m_degree); }
	CD_FORCEINLINE TDegree<T>& operator+=(const TDegree& rhs) const { m_degree += rhs.m_degree; return *this; }

	CD_FORCEINLINE TDegree<T> operator-() const { return TDegree<T>(-m_degree); }
	CD_FORCEINLINE TDegree<T> operator-(const TDegree& rhs) const { return TDegree<T>(m_degree - rhs.m_degree); }
	CD_FORCEINLINE TDegree<T>& operator-=(const TDegree& rhs) const { m_degree -= rhs.m_degree; return *this; }

	CD_FORCEINLINE TDegree<T> operator*(T scalar) const { return TDegree<T>(m_degree * scalar); }
	CD_FORCEINLINE TDegree<T> operator*=(T scalar) const { m_degree *= scalar; return *this; }

	CD_FORCEINLINE TDegree<T> operator/(T scalar) const { return TDegree<T>(m_degree / scalar); }
	CD_FORCEINLINE TDegree<T> operator/=(T scalar) const { m_degree /= scalar; return *this; }

	CD_FORCEINLINE bool operator<(const TDegree& rhs) const { return m_degree < rhs.m_degree; }
	CD_FORCEINLINE bool operator<=(const TDegree& rhs) const { return m_degree <= rhs.m_degree; }
	CD_FORCEINLINE bool operator>=(const TDegree& rhs) const { return !(*this < rhs); }
	CD_FORCEINLINE bool operator>(const TDegree& rhs) const { return !(*this <= rhs); }
	CD_FORCEINLINE bool operator==(const TDegree& rhs) const { return m_degree == rhs.m_degree; }
	CD_FORCEINLINE bool operator!=(const TDegree& rhs) const { return !(*this == rhs); }

private:
	T m_degree;
};

using Degree = TDegree<float>;

static_assert(sizeof(float) == sizeof(Degree));

}