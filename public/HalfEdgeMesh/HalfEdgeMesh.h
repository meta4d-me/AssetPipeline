#pragma once

#include "HalfEdgeMesh/Vertex.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "Scene/ObjectID.h"

#include <list>

namespace cd
{

class Mesh;

namespace hem
{

class HalfEdgeMeshImpl;

}

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

	std::list<hem::Vertex>& GetVertices();
	const std::list<hem::Vertex>& GetVertices() const;

	std::list<hem::Edge>& GetEdges();
	const std::list<hem::Edge>& GetEdges() const;

	std::list<hem::Face>& GetFaces();
	const std::list<hem::Face>& GetFaces() const;

	std::list<hem::HalfEdge>& GetHalfEdges();
	const std::list<hem::HalfEdge>& GetHalfEdges() const;

	// Helpers.
	bool IsTriangleMesh() const;
	bool IsValid() const;
	void Dump() const;

	// Returns the count of boudary faces.
	uint32_t Boundaries() const;
	bool HasBoundary() const { return Boundaries() > 0U; }

	// Add/Remove are basic mesh edit functions which will maintain connectivity data to make correct topology.
	hem::VertexRef AddVertex();
	hem::EdgeRef AddEdge(hem::VertexRef v0, hem::VertexRef v1);
	std::optional<hem::FaceRef> AddFace(const std::vector<hem::HalfEdgeRef>& loop);
	void RemoveVertex(hem::VertexRef vertex);
	void RemoveEdge(hem::EdgeRef edge);
	void RemoveFace(hem::FaceRef face);

	// Rotate non-boundary edge CCW inside its containing faces.
	// Reassign connectivity data but not create or destroy mesh elements.
	// Returns edge after rotation.
	std::optional<hem::EdgeRef> FlipEdge(hem::EdgeRef edge);

	// Split an edge by adding a new vertex in the midpoint of edge.
	// Reassign connectivity data and add a new vertex and edges, faces.
	// Returns new added vertex.
	std::optional<hem::VertexRef> SplitEdge(hem::EdgeRef edge, float t = 0.5f);

	// Collapse an edge by removing one vertex and optional to move another vertex to a new position.
	// It also needs to delete two adjacent faces along the edge. Then reassign connectivity data.
	// Returns the remain vertex.
	std::optional<hem::VertexRef> CollapseEdge(hem::EdgeRef edge, float t = 0.5f);

private:
	hem::HalfEdgeMeshImpl* m_pHalfEdgeMeshImpl;
};

}