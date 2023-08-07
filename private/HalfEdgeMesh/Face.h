#pragma once

#include "ForwardDecls.h"

namespace cd::hem
{

class Face
{
public:
	Face() = delete;
	explicit Face(FaceID id) : m_id(id) { }
	~Face() = default;

	void SetID(FaceID id) { m_id = id; }
	FaceID GetID() const { return m_id; }

	HalfEdgeRef GetHalfEdge() const { return m_halfEdgeRef; }
	void SetHalfEdge(HalfEdgeRef ref) { m_halfEdgeRef = ref; }

private:
	// data
	FaceID m_id;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

}