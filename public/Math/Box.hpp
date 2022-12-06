#pragma once

#include "Core/ISerializable.hpp"
#include "VectorDerived.hpp"

#include <assert.h>

namespace cd
{

/// <summary>
/// Mathematical Box.
/// </summary>
/// <typeparam name="T">Coordinate value type : float, double, ...</typeparam>
/// <typeparam name="N">The number of dimensions.</typeparam>
template<typename T, std::size_t N>
class TBox final : public ISerializable
{
private:
	using PointType = VectorDerived<T, N>;
	using DirectionType = VectorDerived<T, N>;

public:
	explicit constexpr TBox() = default;

	explicit constexpr TBox(T min, T max) : TBox(PointType(min), PointType(max)) {}

	explicit constexpr TBox(PointType min, PointType max)
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

	PointType& Min() { return m_min; }
	PointType& Max() { return m_max; }
	const PointType& Min() const { return m_min; }
	const PointType& Max() const { return m_max; }
	bool Empty() const { return m_min == m_max; }
	PointType GetCenter() const { return m_min + (m_max - m_min) * static_cast<T>(0.5); }
	DirectionType GetExtents() const { return m_max - m_min; }

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

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override
	{
		ImportDataBuffer(fin, m_min.begin());
		ImportDataBuffer(fin, m_max.begin());
	}

	virtual void ExportBinary(std::ofstream& fout) const override
	{
		ExportDataBuffer(fout, m_min.begin(), m_min.size());
		ExportDataBuffer(fout, m_max.begin(), m_max.size());
	}

private:
	PointType m_min;
	PointType m_max;
};

using Box = TBox<float, 3>;

constexpr int size = sizeof(Box);

}