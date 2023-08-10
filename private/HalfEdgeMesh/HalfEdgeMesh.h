#pragma once

#include "ForwardDecls.h"

namespace cd
{

class Mesh;

}

namespace cd::hem
{

class HalfEdgeMesh
{
public:
	static HalfEdgeMesh FromIndexedMesh(const cd::Mesh& mesh);
	static HalfEdgeMesh Cube();

public:
	HalfEdgeMesh() = default;
	HalfEdgeMesh(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh& operator=(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh(HalfEdgeMesh&&) = default;
	HalfEdgeMesh& operator=(HalfEdgeMesh&&) = default;
	~HalfEdgeMesh() = default;

	// HalfEdgeMesh Copy() const;

	VertexRef EmplaceVertex();
	EdgeRef EmplaceEdge();
	FaceRef EmplaceFace(bool isBoundary = false);
	HalfEdgeRef EmplaceHalfEdge();

	void EraseVertex(VertexRef vertex);
	void EraseEdge(EdgeRef edge);
	void EraseFace(FaceRef face);
	void EraseHalfEdge(HalfEdgeRef halfEdge);

private:
	std::list<Vertex> m_vertices;
	std::list<Edge> m_edges;
	std::list<Face> m_faces;
	std::list<HalfEdge> m_halfEdges;

	uint32_t m_nextVertexID = 0U;
	uint32_t m_nextEdgeID = 0U;
	uint32_t m_nextFaceID = 0U;
	uint32_t m_nextHalfEdgeID = 0U;

	std::list<Vertex> m_freeVertices;
	std::list<Edge> m_freeEdges;
	std::list<Face> m_freeFaces;
	std::list<HalfEdge> m_freeHalfEdges;
};

}