#include "ProgressiveMesh/Vertex.h"

namespace cd::pm
{

void Vertex::AddAdjacentVertex(VertexID vertexID)
{
	if (std::find(m_adjacentVertices.begin(), m_adjacentVertices.end(), vertexID) != m_adjacentVertices.end())
	{
		return;
	}

	m_adjacentVertices.push_back(vertexID);
}

void Vertex::AddAdjacentFace(FaceID faceID)
{
	if (std::find(m_adjacentFaces.begin(), m_adjacentFaces.end(), faceID) != m_adjacentFaces.end())
	{
		return;
	}

	m_adjacentFaces.push_back(faceID);
}

}