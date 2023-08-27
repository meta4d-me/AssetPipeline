#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd::hem
{

class CORE_API Face
{
public:
	Face() = delete;
	explicit Face(FaceID id, bool boundary) : m_id(id), m_isBoundary(boundary) { }
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

	bool IsValid() const;

private:
	// data
	FaceID m_id;
	bool m_isBoundary = false;

	// connectivity
	HalfEdgeRef m_halfEdgeRef;
};

inline bool operator<(const FaceRef& lhs, const FaceRef& rhs)
{
	return &*lhs < &*rhs;
}

inline bool operator<(const FaceCRef& lhs, const FaceCRef& rhs)
{
	return &*lhs < &*rhs;
}

}

namespace std
{

template<>
struct hash<cd::hem::FaceRef>
{
	uint64_t operator()(const cd::hem::FaceRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

template<>
struct hash<cd::hem::FaceCRef>
{
	uint64_t operator()(const cd::hem::FaceCRef& key) const
	{
		static const std::hash<decltype(&*key)> h;
		return h(&*key);
	}
};

}