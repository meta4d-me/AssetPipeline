#include "HalfEdgeMesh/HalfEdge.h"

namespace cd::hem
{

bool HalfEdge::Validate() const
{
	return m_cornerUV.Validate() && m_cornerNormal.Validate();
}

}