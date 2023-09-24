#pragma once

#include "Face.h"
#include "Vertex.h"
#include "Scene/ObjectID.h"

namespace cd
{

class Mesh;

namespace pm
{

class CORE_API ProgressiveMeshImpl
{
public:
	ProgressiveMeshImpl();
	ProgressiveMeshImpl(const ProgressiveMeshImpl&) = delete;
	ProgressiveMeshImpl& operator=(const ProgressiveMeshImpl&) = delete;
	ProgressiveMeshImpl(ProgressiveMeshImpl&&);
	ProgressiveMeshImpl& operator=(ProgressiveMeshImpl&&);
	~ProgressiveMeshImpl();

	void FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons);
	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> BuildCollapseOperations();

	uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }
	Vertex& AddVertex(Point position);
	void RemoveVertex(VertexID vertexID);
	void DisconnectAdjcentVertices(VertexID v0ID, VertexID v1ID);
	Vertex& GetVertex(uint32_t index) { return m_vertices[index]; }
	const Vertex& GetVertex(uint32_t index) const { return m_vertices[index]; }

	uint32_t GetFaceCount() const { return static_cast<uint32_t>(m_faces.size()); }
	Face& AddFace(const std::vector<cd::VertexID>& vertexIDs);
	void RemoveFace(FaceID faceID);
	void ReplaceVertexInFace(FaceID faceID, VertexID v0ID, VertexID v1ID);
	Face& GetFace(uint32_t index) { return m_faces[index]; }
	const Face& GetFace(uint32_t index) const { return m_faces[index]; }

	void ComputeNormal(cd::pm::Face& face);
	void ComputeEdgeCollapseCostAtVertex(uint32_t v0Index);
	float ComputeEdgeCollapseCostAtEdge(uint32_t v0Index, uint32_t v1Index);
	Vertex* GetMinimumCostVertex();
	void Collapse(VertexID v0ID, VertexID v1ID);

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
};

}

}