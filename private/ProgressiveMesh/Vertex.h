#pragma once

#include "Container/DynamicArray.hpp"
#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"

#include <cassert>
#include <cfloat>
#include <vector>

namespace cd::pm
{

class Vertex
{
public:
	Vertex() = delete;
	explicit Vertex(cd::VertexID id) : m_id(id) { }
	Vertex(const Vertex&) = default;
	Vertex& operator=(const Vertex&) = default;
	Vertex(Vertex&&) = default;
	Vertex& operator=(Vertex&&) = default;
	~Vertex() = default;

	void SetID(cd::VertexID id) { m_id = id; }
	cd::VertexID GetID() const { return m_id; }

	void SetPosition(Point position) { m_position = cd::MoveTemp(position); }
	cd::Point& GetPosition() { return m_position; }
	const cd::Point& GetPosition() const { return m_position; }

	void AddAdjacentVertex(cd::VertexID vertexID);
	cd::DynamicArray<cd::VertexID>& GetAdjacentVertices() { return m_adjacentVertices; }
	const cd::DynamicArray<cd::VertexID>& GetAdjacentVertices() const { return m_adjacentVertices; }

	void AddAdjacentFace(cd::FaceID faceID);
	cd::DynamicArray<cd::FaceID>& GetAdjacentFaces() { return m_adjacentFaces; }
	const cd::DynamicArray<cd::FaceID>& GetAdjacentFaces() const { return m_adjacentFaces; }

	void SetCollapseCost(float cost) { m_collapseCost = cost; }
	float GetCollapseCost() const { return m_collapseCost; }

	void SetCollapseTarget(cd::VertexID target) { m_collapseTarget = target; }
	cd::VertexID GetCollapseTarget() const { return m_collapseTarget; }

private:
	// data
	cd::VertexID m_id;

	// TODO : Copy data or just choose source data
	cd::Point m_position;

	// connectivity
	cd::DynamicArray<VertexID> m_adjacentVertices;
	cd::DynamicArray<FaceID> m_adjacentFaces;

	// collapse
	float m_collapseCost;
	cd::VertexID m_collapseTarget;
};

}