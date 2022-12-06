#pragma once

#include "VectorDerived.hpp"

namespace cd
{

// Mathematical Ray.
// Normal : direction perpendicular to the plane.
// Distance : distance between (0, 0, 0) and plane along the normal direction.
template<typename T>
class TRay
{
public:
	using TDirection = VectorDerived<T, 3>;
	using TPoint = VectorDerived<T, 3>;

public:
	TRay() = default;
	explicit TRay(TPoint origin, TDirection normal) : m_origin(MoveTemp(origin)), m_normal(MoveTemp(normal)) {}
	TRay(const TRay& rhs) = default;
	TRay& operator=(const TRay& rhs) = default;
	TRay(TRay&& rhs) = default;
	TRay& operator=(TRay&& rhs) = default;
	~TRay() = default;

	bool IsValid() const { return !m_normal.IsNearlyZero(); }
	TPoint GetPointAt(T parameter) const { return m_origin + m_normal * parameter; }
	T GetParameter(const TPoint& point) const { return (point - m_origin).Dot(m_normal); }
	const TDirection& GetNormal() const { return m_normal; }
	const TPoint& GetOrigin() const { return m_origin; }

private:
	TPoint m_origin;
	TDirection m_normal;
};

using Ray = TRay<float>;

static_cast(std::is_standard_layout_v<Ray>&& std::is_trivial_v<Ray>, "Ray needs to implement copy/move constructors in hand.");

}