#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

void HalfEdge::SetNextAndPrev(HalfEdgeRef current, HalfEdgeRef next)
{
	current->SetNext(next);
	next->SetPrev(current);
}

void HalfEdge::SetData(HalfEdgeRef current, HalfEdgeRef twin, HalfEdgeRef next, VertexRef v, EdgeRef e, FaceRef f)
{
	current->SetTwin(twin);
	current->SetNext(next);
	next->SetPrev(current);
	current->SetVertex(v);
	current->SetEdge(e);
	current->SetFace(f);
}

bool HalfEdge::IsValid() const
{
	return m_cornerUV.IsValid() && m_cornerNormal.IsValid();
}

}