#pragma once

#include "VectorDerived.hpp"

namespace cd
{

// Mathematical Plane.
// Normal : direction perpendicular to the plane.
// Distance : distance between (0, 0, 0) and plane along the normal direction.
template<typename T>
class TPlane
{
public:
	using TDirection = VectorDerived<T, 3>;
	using TPoint = VectorDerived<T, 3>;

public:
	TPlane() = default;
	explicit TPlane(TDirection normal, float w) : m_distance(w), m_normal(MoveTemp(normal)) {}
	// It is a little trick that we pass TPoint a as TDirection base because TPoint a - TPoint (0, 0, 0) is still same.
	explicit TPlane(const TPoint& a, const TPoint& b, const TPoint& c) : TPlane(a, (b - a).Cross(c - a).Normalize()) {}
	explicit TPlane(const TDirection& base, TDirection normal) : m_distance(base.Dot(normal)), m_normal(MoveTemp(normal)) {}
	explicit TPlane(float x, float y, float z, float w) : TPlane(TDirection(x, y, z), w) {}
	TPlane(const TPlane& rhs) = default;
	TPlane& operator=(const TPlane& rhs) = default;
	TPlane(TPlane&& rhs) = default;
	TPlane& operator=(TPlane && rhs) = default;
	~TPlane() = default;

	bool IsValid() const { return !m_normal.IsNearlyZero(); }
	const TDirection& GetNormal() const { return m_normal; }
	T GetDistance() const { return m_distance; }
	TPoint GetOrigin() const { return m_normal * m_distance; }

private:
	TDirection m_normal;
	T m_distance;
};

using Plane = TPlane<float>;

static_cast(std::is_standard_layout_v<Plane> && std::is_trivial_v<Plane>, "Plane needs to implement copy/move constructors in hand.");

}