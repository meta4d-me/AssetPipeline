#pragma once

#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/ForwardDecls.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/Vertex.h"
#include "Math/Vector.hpp"

namespace cd
{

class Mesh;

namespace hem
{

class CORE_API HalfEdgeMesh
{
public:
	static HalfEdgeMesh FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons);
	static HalfEdgeMesh FromIndexedMesh(const cd::Mesh& mesh);

public:
	HalfEdgeMesh() = default;
	HalfEdgeMesh(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh& operator=(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh(HalfEdgeMesh&&) = default;
	HalfEdgeMesh& operator=(HalfEdgeMesh&&) = default;
	~HalfEdgeMesh() = default;

	std::list<Vertex>& GetVertices() { return m_vertices; }
	const std::list<Vertex>& GetVertices() const { return m_vertices; }

	std::list<Edge>& GetEdges() { return m_edges; }
	const std::list<Edge>& GetEdges() const { return m_edges; }

	std::list<Face>& GetFaces() { return m_faces; }
	const std::list<Face>& GetFaces() const { return m_faces; }

	std::list<HalfEdge>& GetHalfEdges() { return m_halfEdges; }
	const std::list<HalfEdge>& GetHalfEdges() const { return m_halfEdges; }

	VertexRef EmplaceVertex();
	EdgeRef EmplaceEdge();
	FaceRef EmplaceFace(bool isBoundary = false);
	HalfEdgeRef EmplaceHalfEdge();

	void EraseVertex(VertexRef vertex);
	void EraseEdge(EdgeRef edge);
	void EraseFace(FaceRef face);
	void EraseHalfEdge(HalfEdgeRef halfEdge);

	bool Validate() const;

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

using HalfEdgeMesh = hem::HalfEdgeMesh;

}