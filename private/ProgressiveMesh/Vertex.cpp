#include "Vertex.h"

namespace cd::pm
{

void Vertex::AddAdjacentVertex(VertexID vertexID)
{
	if (m_adjacentVertices.Contains(vertexID))
	{
		return;
	}

	m_adjacentVertices.Add(vertexID);
}

void Vertex::AddAdjacentFace(FaceID faceID)
{
	if (m_adjacentFaces.Contains(faceID))
	{
		return;
	}

	m_adjacentFaces.Add(faceID);
}

}