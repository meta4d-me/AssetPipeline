#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd::hem
{

class CORE_API Vertex
{
public:
	Vertex() = delete;
	explicit Vertex(VertexID id) : m_id(id), m_position(Point::Nan()) { }
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

	bool Validate() const;

private:
	// data
	VertexID m_id;
	Point m_position;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

inline bool operator<(const VertexRef& lhs, const VertexRef& rhs)
{
	return &*lhs < &*rhs;
}

inline bool operator<(const VertexCRef& lhs, const VertexCRef& rhs)
{
	return &*lhs < &*rhs;
}

}

namespace std
{

template<>
struct hash<cd::hem::VertexRef>
{
	uint64_t operator()(const cd::hem::VertexRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

template<>
struct hash<cd::hem::VertexCRef>
{
	uint64_t operator()(const cd::hem::VertexCRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

}