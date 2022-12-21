#pragma once

#include "Base/Template.h"
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
	using Vec = Vec3f;

public:
	TPlane() = default;
	explicit TPlane(Vec normal, float w) : m_distance(w), m_normal(MoveTemp(normal)) {}
	explicit TPlane(const Vec& a, const Vec& b, const Vec& c) : TPlane(a, (b - a).Cross(c - a).Normalize()) {}
	explicit TPlane(const Vec& base, Vec normal) : m_distance(base.Dot(normal)), m_normal(MoveTemp(normal)) {}
	explicit TPlane(float x, float y, float z, float w) : TPlane(Vec(x, y, z), w) {}
	TPlane(const TPlane& rhs) = default;
	TPlane& operator=(const TPlane& rhs) = default;
	TPlane(TPlane&& rhs) = default;
	TPlane& operator=(TPlane && rhs) = default;
	~TPlane() = default;

	bool IsValid() const { return !m_normal.IsNearlyZero(); }
	const Vec& GetNormal() const { return m_normal; }
	T GetDistance() const { return m_distance; }
	Vec GetOrigin() const { return m_normal * m_distance; }

private:
	Vec m_normal;
	T m_distance;
};

using Plane = TPlane<float>;

//static_cast(std::is_standard_layout_v<Plane> && std::is_trivial_v<Plane>, "Plane needs to implement copy/move constructors in hand.");

}