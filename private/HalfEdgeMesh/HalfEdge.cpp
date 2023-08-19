#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

HalfEdgeRef HalfEdge::GetPrev() const
{
	HalfEdgeRef h = GetNext();
	HalfEdgeRef prev = h;
	
	do
	{
		prev = h;
		h = h->GetNext();
	} while (h != GetNext());

	return prev;
}

void HalfEdge::SetData(HalfEdgeRef twin, HalfEdgeRef next, VertexRef v, EdgeRef e, FaceRef f)
{
	m_twinRef = twin;
	m_nextRef = next;
	m_vertexRef = v;
	m_edgeRef = e;
	m_faceRef = f;
}

bool HalfEdge::Validate() const
{
	return m_cornerUV.Validate() && m_cornerNormal.Validate();
}

}