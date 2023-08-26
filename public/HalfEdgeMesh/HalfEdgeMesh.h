#pragma once

#include "HalfEdgeMesh/ForwardDecls.h"

namespace cd
{

class Mesh;

namespace hem
{

//
// HalfEdgeMesh data structure is useful when you need to edit mesh topology, such as adjacent vertices/faces data.
// To implement this requirement, Edge and HalfEdge concepts are added compared to cd::Mesh which is an IndexMesh data structure.
// 
// Features
//		* Two-directional data conversion between cd::Mesh.
//		* Basic mesh edit operations such as Add/Remove Vertex/Edge/Face. HalfEdge operations are internal.
//		* Advanced mesh edit operations such as Flip/Split/CollapseEdge.
// 
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
	bool IsTriangleMesh() const;
	bool IsValid() const;
	void Dump() const;

	// Returns the count of boudary faces.
	uint32_t Boundaries() const;
	bool HasBoundary() const { return Boundaries() > 0U; }

	// Add/Remove are basic mesh edit functions which will maintain connectivity data to make correct topology.
	VertexRef AddVertex();
	EdgeRef AddEdge(VertexRef v0, VertexRef v1);
	std::optional<FaceRef> AddFace(const std::vector<HalfEdgeRef>& loop);
	void RemoveVertex(VertexRef vertex);
	void RemoveEdge(EdgeRef edge);
	void RemoveFace(FaceRef face);

	// Find the first free incident halfedge from begin to end in CCW loop.
	std::optional<HalfEdgeRef> FindFreeIncident(HalfEdgeRef begin, HalfEdgeRef end);

	// Make in halfedge adjacent with out halfedge.
	bool MakeAdjacent(HalfEdgeRef in, HalfEdgeRef out);

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
	// Emplace/Erase functions are only used to allocate/free Vertex/HalfEdge/Edge/Face objects.
	VertexRef EmplaceVertex();
	HalfEdgeRef EmplaceHalfEdge();
	EdgeRef EmplaceEdge();
	FaceRef EmplaceFace(bool isBoundary = false);
	void EraseVertex(VertexRef vertex);
	void EraseHalfEdge(HalfEdgeRef halfEdge);
	void EraseEdge(EdgeRef edge);
	void EraseFace(FaceRef face);

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