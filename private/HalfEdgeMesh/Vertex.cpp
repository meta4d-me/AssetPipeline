#include "Vertex.h"

namespace cd::hem
{

bool Vertex::IsOnBoundary() const
{
	bool isOnBoundary = false;

	//HalfEdgeCRef h = m_halfEdgeRef;
	//do
	//{
	//
	//} while (!isOnBoundary && h != m_halfEdgeRef);
	
	return isOnBoundary;
}

uint32_t Vertex::Degree() const
{
	return 0U;
}

}