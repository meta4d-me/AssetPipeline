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
	return (m_halfEdgeRef->GetVertex()->GetPosition() + m_halfEdgeRef->GetEndVertex()->GetPosition()) * 0.5f;
}

Direction Edge::Normal() const
{
	return (m_halfEdgeRef->GetFace()->Normal() + m_halfEdgeRef->GetTwin()->GetFace()->Normal()).Normalize();
}

float Edge::Length() const
{
	return (m_halfEdgeRef->GetVertex()->GetPosition() - m_halfEdgeRef->GetEndVertex()->GetPosition()).Length();
}

bool Edge::IsValid() const
{
	return true;
}

}