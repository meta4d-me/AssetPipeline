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
/// <typeparam name="T">The value type of Point.</typeparam>
/// <typeparam name="N">The number of dimensions.</typeparam>
template<typename T, std::size_t N>
class TBox final
{
public:
	using PointType = TVector<T, N>;
	static constexpr std::size_t Dimensions = N;

	static TBox<T, N> Empty()
	{
		constexpr T zero = static_cast<T>(0);
		return TBox<T, N>(PointType(zero), PointType(zero));
	}

public:
	explicit constexpr TBox() = default;

	explicit constexpr TBox(T min, T max) : TBox(PointType(min), PointType(max)) {}

	explicit constexpr TBox(PointType min, PointType max)
		: m_min(MoveTemp(min))
		, m_max(MoveTemp(max))
	{
	}

	TBox(const TBox&) = default;
	TBox& operator=(const TBox&) = default;
	TBox(TBox&&) = default;
	TBox& operator=(TBox&&) = default;
	~TBox() = default;

	PointType& Min() { return m_min; }
	PointType& Max() { return m_max; }
	const PointType& Min() const { return m_min; }
	const PointType& Max() const { return m_max; }
	PointType Center() const { return m_min + (m_max - m_min) * static_cast<T>(0.5); }
	PointType Size() const { return m_max - m_min; }
	void Clear() { m_min.Clear(); m_max.Clear(); }

	bool IsEmpty() const { return m_min == m_max; }
	bool IsPointInside(const PointType& point) const
	{
		for(size_t dimensionIndex = 0; dimensionIndex < Dimensions; ++dimensionIndex)
		{
			if(point[dimensionIndex] < m_min[dimensionIndex] ||
				point[dimensionIndex] > m_max[dimensionIndex])
			{
				return false;
			}
		}

		return true;
	}

	void Merge(const TBox& other)
	{
		for (size_t dimensionIndex = 0; dimensionIndex < Dimensions; ++dimensionIndex)
		{
			m_min[dimensionIndex] = std::min<T>(m_min[dimensionIndex], other.Min()[dimensionIndex]);
			m_max[dimensionIndex] = std::max<T>(m_max[dimensionIndex], other.Max()[dimensionIndex]);
		}
	}

	TBox Transform(const cd::Matrix4x4& transform)
	{
		static_assert(3 == N);

		TBox result(*this);

		TVector<T, 4> transformedCenter = transform * cd::TVector<T, 4>(result.Center().x(), result.Center().y(), result.Center().z(), static_cast<T>(1));
		TVector<T, 3> newCenter(transformedCenter.x(), transformedCenter.y(), transformedCenter.z());
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

	bool Intersects(const TRay<T>& ray, T& t) const
	{
		// TODO : For 2D Rect.
		static_assert(3 == N);

		// TODO : have a look at "Fast Ray-Box Intersection" Graphics Gems, 1990.
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
	PointType m_min;
	PointType m_max;
};

using Rect = TBox<float, 2>;
static_assert(4 * sizeof(float) == sizeof(Rect));
static_assert(std::is_standard_layout_v<Rect>&& std::is_trivial_v<Rect>);

using Box = TBox<float, 3>;
static_assert(6 * sizeof(float) == sizeof(Box));
static_assert(std::is_standard_layout_v<Box> && std::is_trivial_v<Box>);

using AABB = Box;

}