#pragma once

#include "ForwardDecls.h"

namespace cd::hem
{

class HalfEdge
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

	void SetEdege(EdgeRef ref) { m_edgeRef = ref; }
	EdgeRef GetEdge() const { return m_edgeRef; }

	void SetFace(FaceRef ref) { m_faceRef = ref; }
	FaceRef GetFace() const { return m_faceRef; }

private:
	// data
	HalfEdgeID m_id;

	// connectivity
	HalfEdgeRef m_twinRef;
	HalfEdgeRef m_nextRef;
	VertexRef m_vertexRef;
	EdgeRef m_edgeRef;
	FaceRef m_faceRef;
};

}