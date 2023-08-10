#pragma once

#include "ForwardDecls.h"

namespace cd::hem
{

class Vertex
{
public:
	Vertex() = delete;
	explicit Vertex(VertexID id) : m_id(id) { }
	Vertex(const Vertex&) = default;
	Vertex& operator=(const Vertex&) = default;
	Vertex(Vertex&&) = default;
	Vertex& operator=(Vertex&&) = default;
	~Vertex() = default;

	void SetID(VertexID id) { m_id = id; }
	VertexID GetID() const { return m_id; }

	void SetPosition(Point position) { m_position = cd::MoveTemp(position); }
	Point& GetPosition() { return m_position; }
	const Point& GetPosition() const { return m_position; }

	HalfEdgeRef GetHalfEdge() const { return m_halfEdgeRef; }
	void SetHalfEdge(HalfEdgeRef ref) { m_halfEdgeRef = ref; }

	bool IsOnBoundary() const;
	Point NeighborCenter() const;
	Direction Normal() const;
	uint32_t Degree() const;

private:
	// data
	VertexID m_id;
	Point m_position;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

}