#pragma once

#include "ProgressiveMesh/ForwardDecls.h"

namespace cd::pm
{

class CORE_API Vertex
{
public:
	Vertex() = delete;
	explicit Vertex(VertexID id) : m_id(id), m_position(Point::Nan()) { }
	Vertex(const Vertex&) = default;
	Vertex& operator=(const Vertex&) = default;
	Vertex(Vertex&&) = default;
	Vertex& operator=(Vertex&&) = default;
	~Vertex() = default;

	void SetID(VertexID id) { m_id = id; }
	VertexID GetID() const { return m_id; }

	void SetPosition(Point position) { m_position = cd::MoveTemp(position); }
	Point& GetPosition() { return m_position; }
	const Point& GetPosition() const { return m_position; }

	void AddAdjacentVertex(VertexID vertexID);
	std::vector<VertexID>& GetAdjacentVertices() { return m_adjacentVertices; }
	const std::vector<VertexID>& GetAdjacentVertices() const { return m_adjacentVertices; }

	void AddAdjacentFace(FaceID faceID);
	std::vector<FaceID>& GetAdjacentFaces() { return m_adjacentFaces; }
	const std::vector<FaceID>& GetAdjacentFaces() const { return m_adjacentFaces; }

	void SetCollapseCost(float cost) { m_collapseCost = cost; }
	float GetCollapseCost() const { return m_collapseCost; }

	void SetCollapseTarget(VertexID target) { m_collapseTarget = target; }
	VertexID GetCollapseTarget() const { return m_collapseTarget; }

private:
	// data
	VertexID m_id;

	// TODO : Copy data or just choose source data
	Point m_position;

	// connectivity
	std::vector<VertexID> m_adjacentVertices;
	std::vector<FaceID> m_adjacentFaces;

	// collapse
	float m_collapseCost;
	VertexID m_collapseTarget;
};

}