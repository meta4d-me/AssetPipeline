#pragma once

#include "ForwardDecls.h"

namespace cd::hem
{

class Face
{
public:
	Face() = delete;
	explicit Face(FaceID id, bool boundary) : m_id(id) { }
	Face(const Face&) = default;
	Face& operator=(const Face&) = default;
	Face(Face&&) = default;
	Face& operator=(Face&&) = default;
	~Face() = default;

	void SetID(FaceID id) { m_id = id; }
	FaceID GetID() const { return m_id; }

	bool IsBoundary() const { return m_isBoundary; }
	void SetIsBoundary(bool isBoundary) { m_isBoundary = isBoundary; }

	HalfEdgeRef GetHalfEdge() const { return m_halfEdgeRef; }
	void SetHalfEdge(HalfEdgeRef ref) { m_halfEdgeRef = ref; }

	Point Center() const;
	Direction Normal() const;
	uint32_t Degree() const;
	float Area() const;

private:
	// data
	FaceID m_id;
	bool m_isBoundary = false;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

}