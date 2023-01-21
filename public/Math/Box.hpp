#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Matrix.hpp"
#include "Math/Ray.hpp"

namespace cd
{

/// <summary>
/// Mathematical Box.
/// </summary>
/// <typeparam name="T">Coordinate value type : float, double, ...</typeparam>
/// <typeparam name="N">The number of dimensions.</typeparam>
template<typename T>
class TBox final
{
public:
	explicit constexpr TBox() = default;

	explicit constexpr TBox(T min, T max) : TBox(TVector<T, 3>(min), TVector<T, 3>(max)) {}

	explicit constexpr TBox(TVector<T, 3> min, TVector<T, 3> max)
		: m_min(MoveTemp(min))
		, m_max(MoveTemp(max))
	{
	}

	TBox(const TBox&) = default;
	TBox& operator=(const TBox&) = default;
	TBox(TBox&&) = default;
	TBox& operator=(TBox&&) = default;
	~TBox() = default;

	TVector<T, 3>& Min() { return m_min; }
	TVector<T, 3>& Max() { return m_max; }
	const TVector<T, 3>& Min() const { return m_min; }
	const TVector<T, 3>& Max() const { return m_max; }
	TVector<T, 3> Center() const { return m_min + (m_max - m_min) * static_cast<T>(0.5); }
	TVector<T, 3> Size() const { return m_max - m_min; }

	bool Empty() const { return m_min == m_max; }
	bool IsPointInside(const Point& point) const { return !(point.x() < m_min.x() || point.x() > m_max.x() || point.y() < m_min.y() || point.y() > m_max.y() || point.z() < m_min.z() || point.z() > m_max.z()); }

	void Merge(const TBox& other)
	{
		for (int index = 0; index < m_min.Size; ++index)
		{
			m_min.x() = std::min(m_min.x(), other.Min().x());
			m_min.y() = std::min(m_min.y(), other.Min().y());
			m_min.z() = std::min(m_min.z(), other.Min().z());

			m_max.x() = std::max(m_max.x(), other.Max().x());
			m_max.y() = std::max(m_max.y(), other.Max().y());
			m_max.z() = std::max(m_max.z(), other.Max().z());
		}
	}

	TBox Transform(const cd::Matrix4x4& transform)
	{
		TBox result(*this);

		TVector<T, 3> newCenter = transform * cd::TVector<T, 4>(result.Center(), static_cast<T>(1));
		TVector<T, 3> oldEdge = result.Size();
		oldEdge *= static_cast<T>(0.5);

		TVector<T, 3> newEdge(
			std::abs(transform.Data(0, 0)) * oldEdge.x() + std::abs(transform.Data(1, 0)) * oldEdge.y() + std::abs(transform.Data(2, 0)) * oldEdge.z(),
			std::abs(transform.Data(0, 1)) * oldEdge.x() + std::abs(transform.Data(1, 1)) * oldEdge.y() + std::abs(transform.Data(2, 1)) * oldEdge.z(),
			std::abs(transform.Data(0, 2)) * oldEdge.x() + std::abs(transform.Data(1, 2)) * oldEdge.y() + std::abs(transform.Data(2, 2)) * oldEdge.z());

		result.Min() = newCenter - newEdge;
		result.Max() = newCenter + newEdge;

		return result;
	}

	// TODO : have a look at "Fast Ray-Box Intersection" Graphics Gems, 1990.
	bool Intersects(const TRay<T>& ray, T& t) const
	{
		// We can cache it inside Ray class to speed up calculations.
		T dirfracx = 1.0f / ray.Direction().x();
		T dirfracy = 1.0f / ray.Direction().y();
		T dirfracz = 1.0f / ray.Direction().z();

		T t1 = (m_min.x() - ray.Origin().x()) * dirfracx;
		T t2 = (m_max.x() - ray.Origin().x()) * dirfracx;
		T t3 = (m_min.y() - ray.Origin().y()) * dirfracy;
		T t4 = (m_max.y() - ray.Origin().y()) * dirfracy;
		T t5 = (m_min.z() - ray.Origin().z()) * dirfracz;
		T t6 = (m_max.z() - ray.Origin().z()) * dirfracz;
		T tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		T tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (tmax < 0)
		{
			t = tmax;
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax)
		{
			t = tmax;
			return false;
		}

		t = tmin;
		return true;
	}

	template<bool SwapBytesOrder>
	TBox& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		inputArchive.ImportBuffer(m_min.Begin());
		inputArchive.ImportBuffer(m_max.Begin());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TBox& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive.ExportBuffer(m_min.Begin(), m_min.Size);
		outputArchive.ExportBuffer(m_max.Begin(), m_max.Size);

		return *this;
	}

private:
	TVector<T, 3> m_min;
	TVector<T, 3> m_max;
};

using Box = TBox<float>;

static_assert(6 * sizeof(float) == sizeof(Box));
static_assert(std::is_standard_layout_v<Box> && std::is_trivial_v<Box>);

}