#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "VectorDerived.hpp"

#include <assert.h>

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
private:
	using Vec = VectorDerived<T, 3>;

public:
	explicit constexpr TBox() = default;

	explicit constexpr TBox(T min, T max) : TBox(Vec(min), Vec(max)) {}

	explicit constexpr TBox(Vec min, Vec max)
		: m_min(MoveTemp(min))
		, m_max(MoveTemp(max))
	{
		assert(!Empty() && "Init an empty Box.");
	}

	TBox(const TBox&) = default;
	TBox& operator=(const TBox&) = default;
	TBox(TBox&&) = default;
	TBox& operator=(TBox&&) = default;
	~TBox() = default;

	Vec& Min() { return m_min; }
	Vec& Max() { return m_max; }
	const Vec& Min() const { return m_min; }
	const Vec& Max() const { return m_max; }
	bool Empty() const { return m_min == m_max; }
	Vec GetCenter() const { return m_min + (m_max - m_min) * static_cast<T>(0.5); }
	Vec GetExtent() const { return m_max - m_min; }

	void Expand(const TBox& other)
	{
		for (int index = 0; index < m_min.size(); ++index)
		{
			m_min.x() = std::min(m_min.x(), other.Min().x());
			m_min.y() = std::min(m_min.y(), other.Min().y());
			m_min.z() = std::min(m_min.z(), other.Min().z());

			m_max.x() = std::max(m_max.x(), other.Max().x());
			m_max.y() = std::max(m_max.y(), other.Max().y());
			m_max.z() = std::max(m_max.z(), other.Max().z());
		}
	}

	template<bool SwapBytesOrder>
	TBox& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		inputArchive.ImportBuffer(m_min.begin());
		inputArchive.ImportBuffer(m_max.begin());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TBox& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive.ExportBuffer(m_min.begin(), m_min.size());
		outputArchive.ExportBuffer(m_max.begin(), m_max.size());

		return *this;
	}

private:
	Vec m_min;
	Vec m_max;
};

using Box = TBox<float>;

//static_assert(std::is_standard_layout_v<Box> && std::is_trivial_v<Box>);

}