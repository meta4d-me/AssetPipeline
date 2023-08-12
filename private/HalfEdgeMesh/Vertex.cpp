#include "HalfEdgeMesh/Vertex.h"

#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

bool Vertex::IsOnBoundary() const
{
	bool isOnBoundary = false;
	HalfEdgeCRef h = m_halfEdgeRef;

	do
	{
		isOnBoundary = isOnBoundary || h->GetFace()->IsBoundary();
		h = h->GetTwin()->GetNext();
	} while (!isOnBoundary && h != m_halfEdgeRef);
	
	return isOnBoundary;
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
		h = h->GetTwin()->GetNext();
	} while (h != m_halfEdgeRef);

	center /= neighborCount;

	return center;
}

Direction Vertex::Normal() const
{
	return Direction::Zero();
}

uint32_t Vertex::Degree() const
{
	uint32_t degree = 0U;
	HalfEdgeCRef h = m_halfEdgeRef;

	do
	{
		++degree;
		h = h->GetTwin()->GetNext();
	} while (h != m_halfEdgeRef);

	return degree;
}

bool Vertex::Validate() const
{
	return cd::Math::Validate(m_position.x()) && cd::Math::Validate(m_position.y()) && cd::Math::Validate(m_position.z());
}

}