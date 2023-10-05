#include "HalfEdgeMesh/Vertex.h"

#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

std::optional<HalfEdgeRef> Vertex::GetHalfEdgeToVertex(VertexRef vertex) const
{
	HalfEdgeRef h = m_halfEdgeRef;
	do
	{
		if (h->GetEndVertex() == vertex)
		{
			return h;
		}

		h = h->GetRotateNext();
	} while (h != m_halfEdgeRef);

	return std::nullopt;
}

bool Vertex::IsOnBoundary() const
{
	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		if (h->GetFace()->IsBoundary())
		{
			return true;
		}

		h = h->GetRotateNext();
	} while (h != m_halfEdgeRef);
	
	return false;
}

Point Vertex::NeighborCenter() const
{
	Point center(0.0f);
	float neighborCount = 0.0f;

	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		center += h->GetNext()->GetVertex()->GetPosition();
		neighborCount += 1.0f;
		h = h->GetRotateNext();
	} while (h != m_halfEdgeRef);

	center /= neighborCount;

	return center;
}

Direction Vertex::Normal() const
{
	Direction normal(0.0f);
	
	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		const Point& v1 = h->GetNext()->GetVertex()->GetPosition();

		h = h->GetRotateNext();

		const Point& v2 = h->GetNext()->GetVertex()->GetPosition();

		if (!h->GetFace()->IsBoundary())
		{
			normal += (v1 - m_position).Cross(v2 - m_position);
		}

	} while (h != m_halfEdgeRef);

	normal.Normalize();
	return normal;
}

uint32_t Vertex::Degree() const
{
	uint32_t degree = 0U;

	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		++degree;
		h = h->GetRotateNext();
	} while (h != m_halfEdgeRef);

	return degree;
}

bool Vertex::IsValid() const
{
	return m_position.IsValid();
}

}