#include "Vertex.h"

namespace cd::pm
{

void Vertex::AddAdjacentVertex(VertexID vertexID)
{
	if (!m_adjacentVertices.Contains(vertexID))
	{
		m_adjacentVertices.Add(vertexID);
	}
}

void Vertex::AddAdjacentFace(FaceID faceID)
{
	if (!m_adjacentFaces.Contains(faceID))
	{
		m_adjacentFaces.Add(faceID);
	}
}

float Vertex::GetCollapseCost() const
{
	return m_collapseCost;
}

}