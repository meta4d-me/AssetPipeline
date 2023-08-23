#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

bool HalfEdge::IsValid() const
{
	return m_cornerUV.IsValid() && m_cornerNormal.IsValid();
}

}