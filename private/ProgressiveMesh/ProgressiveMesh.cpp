#include "ProgressiveMesh/ProgressiveMesh.h"

#include "ProgressiveMesh/Face.h"
#include "ProgressiveMesh/Vertex.h"
#include "Scene/Mesh.h"

namespace
{

cd::Direction ComputeNormal(const cd::pm::ProgressiveMesh& pm, const cd::pm::Face& face)
{
	auto v0Index = face.GetVertexID(0U).Data();
	auto v1Index = face.GetVertexID(1U).Data();
	auto v2Index = face.GetVertexID(2U).Data();

	const auto& v0Position = pm.GetVertex(v0Index).GetPosition();
	const auto& v1Position = pm.GetVertex(v1Index).GetPosition();
	const auto& v2Position = pm.GetVertex(v2Index).GetPosition();

	auto v1v0 = v1Position - v0Position;
	auto v2v0 = v2Position - v0Position;
	return v1v0.Cross(v2v0).Normalize();
}

}

namespace cd::pm
{

ProgressiveMesh::ProgressiveMesh() = default;
ProgressiveMesh::ProgressiveMesh(ProgressiveMesh&&) = default;
ProgressiveMesh& ProgressiveMesh::operator=(ProgressiveMesh&&) = default;
ProgressiveMesh::~ProgressiveMesh() = default;

ProgressiveMesh ProgressiveMesh::FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons)
{
	ProgressiveMesh pm;

	uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		pm.AddVertex(vertices[vertexIndex]);
	}

	for (const auto& polygon : polygons)
	{
		uint32_t v0 = polygon[0].Data();
		uint32_t v1 = polygon[1].Data();
		uint32_t v2 = polygon[2].Data();
		assert(v0 != v1 && v0 != v2 && v1 != v2);
		assert(v0 < vertexCount && v1 < vertexCount && v2 < vertexCount);

		auto& face = pm.AddFace(polygon);
		face.SetNormal(ComputeNormal(pm, face));
		
		pm.GetVertex(v0).AddAdjacentFace(face.GetID());
		pm.GetVertex(v1).AddAdjacentFace(face.GetID());
		pm.GetVertex(v2).AddAdjacentFace(face.GetID());

		pm.GetVertex(v0).AddAdjacentVertex(v1);
		pm.GetVertex(v0).AddAdjacentVertex(v2);
		pm.GetVertex(v1).AddAdjacentVertex(v0);
		pm.GetVertex(v1).AddAdjacentVertex(v2);
		pm.GetVertex(v2).AddAdjacentVertex(v0);
		pm.GetVertex(v2).AddAdjacentVertex(v1);
	}

	for (uint32_t vertexIndex = 0U; vertexIndex < pm.GetVertexCount(); ++vertexIndex)
	{
		pm.ComputeEdgeCollapseCostAtVertex(vertexIndex);
	}
	
	return pm;
}

ProgressiveMesh ProgressiveMesh::FromIndexedMesh(const cd::Mesh& mesh)
{
	return FromIndexedFaces(mesh.GetVertexPositions(), mesh.GetPolygons());
}

Vertex& ProgressiveMesh::AddVertex(Point position)
{
	auto& vertex = m_vertices.emplace_back(GetVertexCount());
	vertex.SetPosition(cd::MoveTemp(position));
	vertex.SetCollapseCost(0.0f);
	vertex.SetCollapseTarget(cd::VertexID::InvalidID);
	return vertex;
}

void ProgressiveMesh::RemoveVertex(uint32_t index)
{
}

Face& ProgressiveMesh::AddFace(const std::vector<cd::VertexID>& vertexIDs)
{
	assert(vertexIDs.size() == 3);
	auto& face = m_faces.emplace_back(GetFaceCount());
	face.SetVertexIDs(vertexIDs);
	return face;
}

void ProgressiveMesh::RemoveFace(uint32_t index)
{
}

void ProgressiveMesh::ReplaceVertexInFace(uint32_t faceIndex, uint32_t v0, uint32_t v1)
{
}

void ProgressiveMesh::ComputeEdgeCollapseCostAtVertex(uint32_t v0Index)
{
	auto& v0 = m_vertices[v0Index];
	if (v0.GetAdjacentVertices().empty())
	{
		v0.SetCollapseCost(-1.0f);
		v0.SetCollapseTarget(cd::VertexID::InvalidID);
		return;
	}

	for (auto v1ID : v0.GetAdjacentVertices())
	{
		float cost = ComputeEdgeCollapseCostAtEdge(v0Index, v1ID.Data());
		if (cost < v0.GetCollapseCost())
		{
			v0.SetCollapseCost(cost);
			v0.SetCollapseTarget(v1ID);
		}
	}
}

float ProgressiveMesh::ComputeEdgeCollapseCostAtEdge(uint32_t v0Index, uint32_t v1Index)
{
	auto& v0 = m_vertices[v0Index];
	auto& v1 = m_vertices[v1Index];

	float edgeLength = (v0.GetPosition() - v1.GetPosition()).Length();
	float curvature = 0.0f;

	std::vector<FaceID> sideFaceIDs;
	for (auto v0Face : v0.GetAdjacentFaces())
	{
		for (auto v1Face : v1.GetAdjacentFaces())
		{
			if (v0Face == v1Face)
			{
				sideFaceIDs.push_back(v0Face);
			}
		}
	}

	for (auto v0Face : v0.GetAdjacentFaces())
	{
		float minCurvature = 1.0f;
		for (auto sideFace : sideFaceIDs)
		{
			const auto& v0FaceNormal = m_faces[v0Face.Data()].GetNormal();
			const auto& sideFaceNormal = m_faces[sideFace.Data()].GetNormal();
			float normalDot = v0FaceNormal.Dot(sideFaceNormal);
			float t = (1.0f - normalDot) * 0.5f;
			if (t < minCurvature)
			{
				minCurvature = t;
			}
		}

		if (minCurvature > curvature)
		{
			curvature = minCurvature;
		}
	}

	return edgeLength * curvature;
}

Vertex* ProgressiveMesh::GetMinimumCostVertex()
{
	Vertex* pCandidate = nullptr;
	uint32_t vertexCount = GetVertexCount();
	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		auto& vertex = m_vertices[vertexIndex];
		if (vertex.GetCollapseTarget().IsValid())
		{
			if (!pCandidate || vertex.GetCollapseCost() < pCandidate->GetCollapseCost())
			{
				pCandidate = &vertex;
			}
		}
	}

	return pCandidate;
}

void ProgressiveMesh::Collapse(uint32_t v0Index, uint32_t v1Index)
{
	if (v1Index == cd::VertexID::InvalidID)
	{
		RemoveVertex(v0Index);
		return;
	}

	auto& v0 = m_vertices[v0Index];
	std::vector<cd::VertexID> tmp = v0.GetAdjacentVertices();

	for (auto faceID : v0.GetAdjacentFaces())
	{
		auto& face = m_faces[faceID.Data()];
		auto& faceVertexIDs = face.GetVertexIDs();
		if (std::find(faceVertexIDs.begin(), faceVertexIDs.end(), VertexID(v1Index)) != faceVertexIDs.end())
		{
			RemoveFace(faceID.Data());
		}
	}

	for (auto faceID : v0.GetAdjacentFaces())
	{
		uint32_t faceIndex = faceID.Data();
		auto& face = m_faces[faceIndex];
		ReplaceVertexInFace(faceIndex, v0Index, v1Index);
	}

	RemoveVertex(v0Index);

	for (auto vertexID : tmp)
	{
		ComputeEdgeCollapseCostAtVertex(vertexID.Data());
	}
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> ProgressiveMesh::BuildCollapseOperations()
{
	uint32_t vertexCount = GetVertexCount();
	std::vector<uint32_t> permutation;
	permutation.resize(vertexCount);

	std::vector<uint32_t> map;
	map.resize(vertexCount);

	for (uint32_t vertexIndex = vertexCount - 1; vertexIndex >= 0U; --vertexIndex)
	{
		Vertex* pCandidate = GetMinimumCostVertex();
		permutation[pCandidate->GetID().Data()] = vertexIndex;
		map[vertexIndex] = pCandidate->GetCollapseTarget().Data();

		Collapse(pCandidate->GetID().Data(), pCandidate->GetCollapseTarget().Data());
	}

	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		map[vertexIndex] = map[vertexIndex] == cd::VertexID::InvalidID ? 0U : permutation[map[vertexIndex]];
	}

	return std::make_pair(cd::MoveTemp(permutation), cd::MoveTemp(map));
}

}