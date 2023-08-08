#pragma once

#include "ForwardDecls.h"

namespace cd::hem
{

class Edge
{
public:
	Edge() = delete;
	explicit Edge(EdgeID id) : m_id(id) { }
	~Edge() = default;

	void SetID(EdgeID id) { m_id = id; }
	EdgeID GetID() const { return m_id; }

	HalfEdgeRef GetHalfEdge() const { return m_halfEdgeRef; }
	void SetHalfEdge(HalfEdgeRef ref) { m_halfEdgeRef = ref; }

	bool IsOnBoundary() const;
	Point Center() const;
	float Length() const;

private:
	// data
	EdgeID m_id;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

}