#pragma once

#include "Face.h"
#include "Vertex.h"
#include "Scene/ObjectID.h"

#include <set>

namespace cd
{

class Mesh;

namespace pm
{

struct CompareVertexCollapseCost
{
	bool operator()(const Vertex* pV0, const Vertex* pV1) const
	{
		float v0Cost = pV0->GetCollapseCost();
		float v1Cost = pV1->GetCollapseCost();
		//printf("Compare [Vertex %u] and [Vertex %u]\n", pV0->GetID().Data(), pV1->GetID().Data());
		//printf("\t[Vertex %u] cost is %f\n", pV0->GetID().Data(), v0Cost);
		//printf("\t[Vertex %u] cost is %f\n", pV1->GetID().Data(), v1Cost);
		return v0Cost == v1Cost ? pV0->GetID() < pV1->GetID() : v0Cost < v1Cost;
	}
};

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
	void InitBoundary(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons);
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
	void ComputeEdgeCollapseCostAtVertex(VertexID v0ID);
	float ComputeEdgeCollapseCostAtEdge(VertexID v0ID, VertexID v1ID);
	void Collapse(VertexID v0ID, VertexID v1ID);

	cd::Mesh GenerateLodMesh(float percent);
	cd::Mesh GenerateLodMesh(uint32_t targetFaceCount);

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
	std::multiset<Vertex*, CompareVertexCollapseCost> m_minCostVertexQueue;
};

}

}