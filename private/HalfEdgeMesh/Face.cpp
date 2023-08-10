#include "Face.h"

#include "HalfEdge.h"
#include "Vertex.h"

namespace cd::hem
{

Point Face::Center() const
{
	Point center(0.0f);
	float vertexCount = 0.0f;

	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		center += h->GetVertex()->GetPosition();
		vertexCount += 1.0f;
		h = h->GetNext();
	} while (h != m_halfEdgeRef);

	center /= vertexCount;
	return center;
}

Direction Face::Normal() const
{
	return Direction::Zero();
}

uint32_t Face::Degree() const
{
	uint32_t degree = 0U;

	HalfEdgeCRef h = m_halfEdgeRef;
	do
	{
		++degree;
		h = h->GetNext();
	} while (h != m_halfEdgeRef);

	return degree;
}

float Face::Area() const
{
	float area = 0.0f;
	HalfEdgeCRef h = m_halfEdgeRef;
	Point v0 = h->GetVertex()->GetPosition();
	h = h->GetNext();

	do
	{
		Direction v1v0 = h->GetVertex()->GetPosition() - v0;
		Direction v2v0 = h->GetNext()->GetVertex()->GetPosition() - v0;
		area += v1v0.Cross(v2v0).Length() * 0.5f;
		h = h->GetNext();
	} while (h != m_halfEdgeRef);

	return area;
}

}