#pragma once

#include "ProgressiveMesh/ForwardDecls.h"

namespace cd
{

class Mesh;

namespace pm
{

class CORE_API ProgressiveMesh
{
public:
	static ProgressiveMesh FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons);
	static ProgressiveMesh FromIndexedMesh(const cd::Mesh& mesh);

public:
	ProgressiveMesh();
	ProgressiveMesh(const ProgressiveMesh&) = delete;
	ProgressiveMesh& operator=(const ProgressiveMesh&) = delete;
	ProgressiveMesh(ProgressiveMesh&&);
	ProgressiveMesh& operator=(ProgressiveMesh&&);
	~ProgressiveMesh();
	
	uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_vertices.size()); }
	Vertex& AddVertex(Point position);
	void RemoveVertex(uint32_t index);
	Vertex& GetVertex(uint32_t index) { return m_vertices[index]; }
	const Vertex& GetVertex(uint32_t index) const { return m_vertices[index]; }
	
	uint32_t GetFaceCount() const { return static_cast<uint32_t>(m_faces.size()); }
	Face& AddFace(const std::vector<cd::VertexID>& vertexIDs);
	void RemoveFace(uint32_t index);
	void ReplaceVertexInFace(uint32_t faceIndex, uint32_t v0, uint32_t v1);
	Face& GetFace(uint32_t index) { return m_faces[index]; }
	const Face& GetFace(uint32_t index) const { return m_faces[index]; }

	void ComputeEdgeCollapseCostAtVertex(uint32_t v0Index);
	float ComputeEdgeCollapseCostAtEdge(uint32_t v0Index, uint32_t v1Index);

	Vertex* GetMinimumCostVertex();
	void Collapse(uint32_t v0Index, uint32_t v1Index);
	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> BuildCollapseOperations();

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
};

}

}