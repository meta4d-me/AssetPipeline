#include "HalfEdgeMesh/Edge.h"

#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/Vertex.h"

namespace cd::hem
{

bool Edge::IsOnBoundary() const
{
	return m_halfEdgeRef->GetFace()->IsBoundary() || m_halfEdgeRef->GetTwin()->GetFace()->IsBoundary();
}

Point Edge::Center() const
{
	return (m_halfEdgeRef->GetVertex()->GetPosition() + m_halfEdgeRef->GetTwin()->GetVertex()->GetPosition()) * 0.5f;
}

Direction Edge::Normal() const
{
	return Direction::Zero();
}

float Edge::Length() const
{
	return (m_halfEdgeRef->GetVertex()->GetPosition() - m_halfEdgeRef->GetTwin()->GetVertex()->GetPosition()).Length();
}

bool Edge::Validate() const
{
	return true;
}

}