#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd::hem
{

class CORE_API HalfEdge
{
public:
	HalfEdge() = delete;
	explicit HalfEdge(HalfEdgeID id) : m_id(id) { }
	HalfEdge(const HalfEdge&) = default;
	HalfEdge& operator=(const HalfEdge&) = default;
	HalfEdge(HalfEdge&&) = default;
	HalfEdge& operator=(HalfEdge&&) = default;
	~HalfEdge() = default;

	void SetID(HalfEdgeID id) { m_id = id; }
	HalfEdgeID GetID() const { return m_id; }

	void SetTwin(HalfEdgeRef ref) { m_twinRef = ref; }
	HalfEdgeRef GetTwin() const { return m_twinRef; }
	
	void SetNext(HalfEdgeRef ref) { m_nextRef = ref; }
	HalfEdgeRef GetNext() const { return m_nextRef; }

	void SetVertex(VertexRef ref) { m_vertexRef = ref; }
	VertexRef GetVertex() const { return m_vertexRef; }

	void SetEdge(EdgeRef ref) { m_edgeRef = ref; }
	EdgeRef GetEdge() const { return m_edgeRef; }

	void SetFace(FaceRef ref) { m_faceRef = ref; }
	FaceRef GetFace() const { return m_faceRef; }

	void SetCornerUV(cd::UV uv) { m_cornerUV = cd::MoveTemp(uv); }
	cd::UV& GetCornerUV() { return m_cornerUV; }
	const cd::UV& GetCornerUV() const { return m_cornerUV; }

	void SetCornerNormal(cd::Direction normal) { m_cornerNormal = cd::MoveTemp(normal); }
	cd::Direction& GetCornerNormal() { return m_cornerNormal; }
	const cd::Direction& GetCornerNormal() const { return m_cornerNormal; }

	bool Validate() const;

private:
	// data
	HalfEdgeID m_id;
	cd::UV m_cornerUV = cd::UV::Zero();
	cd::Direction m_cornerNormal = cd::Direction::Zero();

	// connectivity
	HalfEdgeRef m_twinRef;
	HalfEdgeRef m_nextRef;
	VertexRef m_vertexRef;
	EdgeRef m_edgeRef;
	FaceRef m_faceRef;
};

inline bool operator<(const HalfEdgeRef& lhs, const HalfEdgeRef& rhs)
{
	return &*lhs < &*rhs;
}

inline bool operator<(const HalfEdgeCRef& lhs, const HalfEdgeCRef& rhs)
{
	return &*lhs < &*rhs;
}

}

namespace std
{

template<>
struct hash<cd::hem::HalfEdgeRef>
{
	uint64_t operator()(const cd::hem::HalfEdgeRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

template<>
struct hash<cd::hem::HalfEdgeCRef>
{
	uint64_t operator()(const cd::hem::HalfEdgeCRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

}