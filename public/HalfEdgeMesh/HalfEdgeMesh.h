#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

#include <optional>

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
	HalfEdgeMesh();
	HalfEdgeMesh(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh& operator=(const HalfEdgeMesh&) = delete;
	HalfEdgeMesh(HalfEdgeMesh&&);
	HalfEdgeMesh& operator=(HalfEdgeMesh&&);
	~HalfEdgeMesh();

	std::list<Vertex>& GetVertices() { return m_vertices; }
	const std::list<Vertex>& GetVertices() const { return m_vertices; }

	std::list<Edge>& GetEdges() { return m_edges; }
	const std::list<Edge>& GetEdges() const { return m_edges; }

	std::list<Face>& GetFaces() { return m_faces; }
	const std::list<Face>& GetFaces() const { return m_faces; }

	std::list<HalfEdge>& GetHalfEdges() { return m_halfEdges; }
	const std::list<HalfEdge>& GetHalfEdges() const { return m_halfEdges; }

	// Helpers.
	bool IsValid() const;
	void Dump() const;

	bool IsTriangleMesh() const;

	// Returns the count of boudary faces.
	uint32_t Boundaries() const;
	bool HasBoundary() const { return Boundaries() > 0U; }

	// Emplace/Erase functions are only used to allocate/free Vertex/HalfEdge/Edge/Face objects.
	VertexRef EmplaceVertex();
	HalfEdgeRef EmplaceHalfEdge();
	EdgeRef EmplaceEdge();
	FaceRef EmplaceFace(bool isBoundary = false);
	void EraseVertex(VertexRef vertex);
	void EraseHalfEdge(HalfEdgeRef halfEdge);
	void EraseEdge(EdgeRef edge);
	void EraseFace(FaceRef face);

	// Add/Remove functions will maintain connectivity data to make correct topology.
	VertexRef AddVertex();
	HalfEdgeRef AddHalfEdge();
	EdgeRef AddEdge();
	FaceRef AddFace(bool isBoundary = false);
	void RemoveVertex(VertexRef vertex);
	void RemoveHalfEdge(HalfEdgeRef halfEdge);
	void RemoveEdge(EdgeRef edge);
	void RemoveFace(FaceRef face);

	// Rotate non-boundary edge CCW inside its containing faces.
	// Reassign connectivity data but not create or destroy mesh elements.
	// Returns edge after rotation.
	std::optional<EdgeRef> FlipEdge(EdgeRef edge);

	// Split an edge by adding a new vertex in the midpoint of edge.
	// Reassign connectivity data and add a new vertex and edges, faces.
	// Returns new added vertex.
	std::optional<VertexRef> SplitEdge(EdgeRef edge, float t = 0.5f);

	// Collapse an edge by removing one vertex and optional to move another vertex to a new position.
	// It also needs to delete two adjacent faces along the edge. Then reassign connectivity data.
	// Returns the remain vertex.
	std::optional<VertexRef> CollapseEdge(EdgeRef edge, float t = 0.5f);

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

}