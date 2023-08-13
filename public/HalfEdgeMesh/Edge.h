#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd::hem
{

class CORE_API Edge
{
public:
	Edge() = delete;
	explicit Edge(EdgeID id) : m_id(id) { }
	Edge(const Edge&) = default;
	Edge& operator=(const Edge&) = default;
	Edge(Edge&&) = default;
	Edge& operator=(Edge&&) = default;
	~Edge() = default;

	void SetID(EdgeID id) { m_id = id; }
	EdgeID GetID() const { return m_id; }

	HalfEdgeRef GetHalfEdge() const { return m_halfEdgeRef; }
	void SetHalfEdge(HalfEdgeRef ref) { m_halfEdgeRef = ref; }

	bool IsOnBoundary() const;
	Point Center() const;
	Direction Normal() const;
	float Length() const;

	bool Validate() const;

private:
	// data
	EdgeID m_id;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

}

namespace std
{

template<>
struct hash<cd::hem::EdgeRef>
{
	uint64_t operator()(const cd::hem::EdgeRef& value) const
	{
		return reinterpret_cast<uint64_t>(&value);
	}
};

template<>
struct hash<cd::hem::EdgeCRef>
{
	uint64_t operator()(const cd::hem::EdgeCRef& value) const
	{
		return reinterpret_cast<uint64_t>(&value);
	}
};

}