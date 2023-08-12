#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd::hem
{

class Edge
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