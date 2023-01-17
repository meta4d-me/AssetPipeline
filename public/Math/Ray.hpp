#pragma once

#include "Vector.hpp"

namespace cd
{

// Mathematical Ray.
template<typename T>
class TRay
{
public:
	using TDirection = TVector<T, 3>;
	using TPoint = TVector<T, 3>;

public:
	TRay() = default;
	explicit TRay(TPoint origin, TDirection direction) : m_origin(MoveTemp(origin)), m_direction(MoveTemp(direction)) {}
	TRay(const TRay& rhs) = default;
	TRay& operator=(const TRay& rhs) = default;
	TRay(TRay&& rhs) = default;
	TRay& operator=(TRay&& rhs) = default;
	~TRay() = default;

	bool IsValid() const { return !m_direction.IsZero() && !m_direction.IsNaN(); }
	TPoint GetPointAt(T parameter) const { return m_origin + m_direction * parameter; }
	T GetParameter(const TPoint& point) const { return (point - m_origin).Dot(m_direction); }
	const TDirection& Direction() const { return m_direction; }
	const TPoint& Origin() const { return m_origin; }

private:
	TPoint m_origin;
	TDirection m_direction;
};

using Ray = TRay<float>;

static_assert(6 * sizeof(float) == sizeof(Ray));
//static_cast(std::is_standard_layout_v<Ray> && std::is_trivial_v<Ray>);

}