#include "HalfEdgeMesh.h"

#include "Edge.h"
#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"

namespace cd::hem
{

HalfEdgeMesh HalfEdgeMesh::FromIndexedMesh(const cd::Mesh& mesh)
{
	HalfEdgeMesh halfEdgeMesh;

	return halfEdgeMesh;
}

HalfEdgeMesh HalfEdgeMesh::Cube()
{
	HalfEdgeMesh halfEdgeMesh;

	return halfEdgeMesh;
}

VertexRef HalfEdgeMesh::EmplaceVertex()
{
	VertexRef vertex;
	if (m_freeVertices.empty())
	{
		vertex = m_vertices.emplace(m_vertices.end(), Vertex(m_nextVertexID++));
	}
	else
	{
		vertex = m_freeVertices.begin();
		m_vertices.splice(m_vertices.end(), m_freeVertices, m_freeVertices.begin());
		*vertex = Vertex(m_nextVertexID++);
	}

	vertex->SetHalfEdge(m_halfEdges.end());
	return vertex;
}

EdgeRef HalfEdgeMesh::EmplaceEdge()
{
	EdgeRef edge;
	if (m_freeEdges.empty())
	{
		edge = m_edges.emplace(m_edges.end(), Edge(m_nextEdgeID++));
	}
	else
	{
		edge = m_edges.begin();
		m_edges.splice(m_edges.end(), m_freeEdges, m_freeEdges.begin());
		*edge = Edge(m_nextEdgeID++);
	}

	edge->SetHalfEdge(m_halfEdges.end());
	return edge;
}

FaceRef HalfEdgeMesh::EmplaceFace(bool isBoundary)
{
	FaceRef face;
	if (m_freeFaces.empty())
	{
		face = m_faces.emplace(m_faces.end(), Face(m_nextFaceID++, isBoundary));
	}
	else
	{
		face = m_faces.begin();
		m_faces.splice(m_faces.end(), m_freeFaces, m_freeFaces.begin());
		*face = Face(m_nextFaceID++, isBoundary);
	}

	face->SetHalfEdge(m_halfEdges.end());
	return face;
}

HalfEdgeRef HalfEdgeMesh::EmplaceHalfEdge()
{
	HalfEdgeRef halfEdge;
	if (m_freeHalfEdges.empty())
	{
		halfEdge = m_halfEdges.emplace(m_halfEdges.end(), HalfEdge(m_nextHalfEdgeID++));
	}
	else
	{
		halfEdge = m_halfEdges.begin();
		m_halfEdges.splice(m_halfEdges.end(), m_freeHalfEdges, m_freeHalfEdges.begin());
		*halfEdge = HalfEdge(m_nextHalfEdgeID++);
	}

	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());
	return halfEdge;
}

void HalfEdgeMesh::EraseVertex(VertexRef vertex)
{
	// clear data
	vertex->SetID(VertexID::Invalid());
	vertex->SetPosition(Point::Nan());

	// clear connectivity
	vertex->SetHalfEdge(m_halfEdges.end());

	m_freeVertices.splice(m_freeVertices.end(), m_vertices, vertex);
}

void HalfEdgeMesh::EraseEdge(EdgeRef edge)
{
	// clear data
	edge->SetID(EdgeID::Invalid());

	// clear connectivity
	edge->SetHalfEdge(m_halfEdges.end());

	m_freeEdges.splice(m_freeEdges.end(), m_edges, edge);
}

void HalfEdgeMesh::EraseFace(FaceRef face)
{
	// clear data
	face->SetID(FaceID::Invalid());
	face->SetIsBoundary(false);

	// clear connectivity
	face->SetHalfEdge(m_halfEdges.end());

	m_freeFaces.splice(m_freeFaces.end(), m_faces, face);
}

void HalfEdgeMesh::EraseHalfEdge(HalfEdgeRef halfEdge)
{
	// clear data
	halfEdge->SetID(HalfEdgeID::Invalid());

	// clear connectivity
	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());

	m_freeHalfEdges.splice(m_freeHalfEdges.end(), m_halfEdges, halfEdge);
}

}