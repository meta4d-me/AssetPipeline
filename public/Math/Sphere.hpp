#pragma once

#include "VectorDerived.hpp"

namespace cd
{

// Mathematical Sphere.
// Center : center point.
// Radius : the length of radius.
template<typename T>
class TSphere
{
public:
	using TDirection = VectorDerived<T, 3>;
	using TPoint = VectorDerived<T, 3>;

public:
	TSphere() = default;
	explicit TSphere(TPoint center, float radius) : m_center(center), m_radius(radius) {}
	TSphere(const TSphere& rhs) = default;
	TSphere& operator=(const TSphere& rhs) = default;
	TSphere(TSphere&& rhs) = default;
	TSphere& operator=(TSphere&& rhs) = default;
	~TSphere() = default;

	bool IsValid() const { return m_radius > SmallNumberTolerance; }
	bool IsPointInside(const TPoint& point) { return (point - m_center).LengthSquare() <= (m_radius + SmallNumberTolerance) * (m_radius + SmallNumberTolerance); }
	TPoint GetCenter() const { return m_center; }
	T GetRadius() const { return m_radius; }
	T GetVolume() const { return static_cast<T>(4) / static_cast<T>(3) * PI * m_radius * m_radius * m_radius; }

private:
	TPoint m_center;
	T m_radius;
};

using Sphere = TSphere<float>;

//static_cast(std::is_standard_layout_v<Sphere>&& std::is_trivial_v<Sphere>, "Sphere needs to implement copy/move constructors in hand.");

}