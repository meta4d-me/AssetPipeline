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

bool HalfEdge::Validate() const
{
	return m_cornerUV.Validate() && m_cornerNormal.Validate();
}

}