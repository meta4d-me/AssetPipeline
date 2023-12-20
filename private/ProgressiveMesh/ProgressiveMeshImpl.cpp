#include "ProgressiveMeshImpl.h"

#include "Hashers/HashCombine.hpp"
#include "Scene/Mesh.h"
#include "Scene/VertexFormat.h"

#include <cfloat>
#include <unordered_map>

namespace cd::pm
{

ProgressiveMeshImpl::ProgressiveMeshImpl() = default;
ProgressiveMeshImpl::ProgressiveMeshImpl(ProgressiveMeshImpl&&) = default;
ProgressiveMeshImpl& ProgressiveMeshImpl::operator=(ProgressiveMeshImpl&&) = default;
ProgressiveMeshImpl::~ProgressiveMeshImpl() = default;

void ProgressiveMeshImpl::FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<cd::PolygonGroup>& polygonGroups)
{
	uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		AddVertex(vertices[vertexIndex]);
	}

	for (const auto& polygonGroup : polygonGroups)
	{
		for (const auto& polygon : polygonGroup)
		{
			uint32_t v0Index = polygon[0].Data();
			uint32_t v1Index = polygon[1].Data();
			uint32_t v2Index = polygon[2].Data();
			assert(v0Index != v1Index && v0Index != v2Index && v1Index != v2Index);
			assert(v0Index < vertexCount && v1Index < vertexCount && v2Index < vertexCount);

			auto& face = AddFace(polygon);
			ComputeNormal(face);

			auto& v0 = GetVertex(v0Index);
			auto& v1 = GetVertex(v1Index);
			auto& v2 = GetVertex(v2Index);

			v0.AddAdjacentFace(face.GetID());
			v1.AddAdjacentFace(face.GetID());
			v2.AddAdjacentFace(face.GetID());

			v0.AddAdjacentVertex(v1.GetID());
			v0.AddAdjacentVertex(v2.GetID());
			v1.AddAdjacentVertex(v0.GetID());
			v1.AddAdjacentVertex(v2.GetID());
			v2.AddAdjacentVertex(v0.GetID());
			v2.AddAdjacentVertex(v1.GetID());
		}
	}
}

void ProgressiveMeshImpl::InitBoundary(const cd::AABB& aabb)
{
	for (auto& v : m_vertices)
	{
		if (v.IsOnBoundary())
		{
			continue;
		}
		
		bool isOnBoundary = aabb.IsPointInside(v.GetPosition());
		v.SetIsOnBoundary(isOnBoundary);
	}
}

void ProgressiveMeshImpl::InitBoundary(const std::vector<cd::Point>& vertices, const std::vector<cd::PolygonGroup>& polygonGroups)
{
	auto GetVertexHash = [](const cd::Point& p)
	{
		return HashCombine(Math::CastFloatToU32(p.x()), HashCombine(Math::CastFloatToU32(p.y()), Math::CastFloatToU32(p.z())));
	};

	std::unordered_map<uint32_t, uint32_t> mapBoundaryVertices;
	for (uint32_t vertexIndex = 0U; vertexIndex < vertices.size(); ++vertexIndex)
	{
		uint32_t vertexHash = GetVertexHash(vertices[vertexIndex]);
		//assert(!mapBoundaryVertices.contains(vertexHash));
		mapBoundaryVertices[vertexHash] = vertexIndex;
	}

	for (auto& v : m_vertices)
	{
		if (v.IsOnBoundary())
		{
			continue;
		}

		uint32_t vertexHash = GetVertexHash(v.GetPosition());
		bool isOnBoundary = mapBoundaryVertices.find(vertexHash) != mapBoundaryVertices.end();
		v.SetIsOnBoundary(isOnBoundary);
	}
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> ProgressiveMeshImpl::BuildCollapseOperations()
{
	for (const auto& vertex : m_vertices)
	{
		ComputeEdgeCollapseCostAtVertex(vertex.GetID());
	}

	for (auto& vertex : m_vertices)
	{
		assert(vertex.GetID().IsValid());
		m_minCostVertexQueue.insert(&vertex);
	}

	uint32_t vertexCount = GetVertexCount();
	std::vector<uint32_t> permutation;
	permutation.resize(vertexCount);

	std::vector<uint32_t> map;
	map.resize(vertexCount);

	for (int vertexIndex = static_cast<int>(vertexCount) - 1; vertexIndex >= 0; --vertexIndex)
	{
		assert(!m_minCostVertexQueue.empty());
		auto itMinVertex = m_minCostVertexQueue.begin();
		Vertex* pCandidate = *itMinVertex;
		m_minCostVertexQueue.erase(itMinVertex);

		assert(pCandidate);
		permutation[pCandidate->GetID().Data()] = vertexIndex;
		map[vertexIndex] = pCandidate->GetCollapseTarget().Data();

		//printf("Collapse [Vertex %d] - [Vertex %d], cost = %f\n", pCandidate->GetID().Data(), pCandidate->GetCollapseTarget().Data(), pCandidate->GetCollapseCost());
		Collapse(pCandidate->GetID(), pCandidate->GetCollapseTarget());
	}

	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		map[vertexIndex] = map[vertexIndex] == cd::VertexID::InvalidID ? 0U : permutation[map[vertexIndex]];
	}

	return std::make_pair(cd::MoveTemp(permutation), cd::MoveTemp(map));
}

Vertex& ProgressiveMeshImpl::AddVertex(Point position)
{
	auto& vertex = m_vertices.emplace_back(Vertex(GetVertexCount()));
	vertex.SetPosition(cd::MoveTemp(position));
	vertex.SetCollapseCost(FLT_MAX);
	vertex.SetCollapseTarget(cd::VertexID::InvalidID);
	return vertex;
}

void ProgressiveMeshImpl::RemoveVertex(VertexID vertexID)
{
	auto& v = m_vertices[vertexID.Data()];
	assert(v.GetID() == vertexID);
	assert(v.GetAdjacentFaces().Size() == 0);
	v.SetID(cd::VertexID::InvalidID);
	v.GetAdjacentVertices().Clear();
}

void ProgressiveMeshImpl::DisconnectAdjcentVertices(VertexID v0ID, VertexID v1ID)
{
	if (!v0ID.IsValid() || !v1ID.IsValid())
	{
		return;
	}

	auto DisconnectImpl = [&](ProgressiveMeshImpl* pm, VertexID v0ID, VertexID v1ID)
	{
		auto& v0 = GetVertex(v0ID.Data());
		assert(v0.GetID() == v0ID);
		assert(GetVertex(v1ID.Data()).GetID() == v1ID);

		auto& v0AdjVertices = v0.GetAdjacentVertices();
		for (std::size_t i = 0; i < v0AdjVertices.Size(); ++i)
		{
			auto v0AdjVertexID = v0AdjVertices[i];
			if (v0AdjVertexID == v1ID)
			{
				for (auto v0AdjFaceID : v0.GetAdjacentFaces())
				{
					auto& v0AdjFace = pm->GetFace(v0AdjFaceID.Data());
					//printf("\tQuery face [%d, %d, %d] : \n", v0AdjFace.GetVertexID(0).Data(), v0AdjFace.GetVertexID(1).Data(), v0AdjFace.GetVertexID(2).Data());
					if (v0AdjFace.GetVertexIDs().Contains(v1ID))
					{
						return;
					}
				}

				v0AdjVertices.RemoveByIndex(i);
				//printf("RemoveIfNonNeighbor_2 vertex [%d] adj vertex [%d] : \n", v0ID.Data(), v1ID.Data());
				return;
			}
		}
	};

	DisconnectImpl(this, v0ID, v1ID);
	DisconnectImpl(this, v1ID, v0ID);
}

Face& ProgressiveMeshImpl::AddFace(const std::vector<cd::VertexID>& vertexIDs)
{
	assert(vertexIDs.size() == 3);
	auto& face = m_faces.emplace_back(Face(GetFaceCount()));
	for (auto vertexID : vertexIDs)
	{
		face.GetVertexIDs().Add(vertexID);
	}
	return face;
}

void ProgressiveMeshImpl::RemoveFace(FaceID faceID)
{
	auto& face = m_faces[faceID.Data()];
	assert(face.GetID() == faceID);

	VertexID polygon[3];
	for (uint32_t index = 0U; index < 3U; ++index)
	{
		VertexID vertexID = face.GetVertexID(index);
		auto& vertex = GetVertex(vertexID.Data());

		if (vertex.GetID().IsValid())
		{
			polygon[index] = vertex.GetID();
			vertex.GetAdjacentFaces().RemoveByValue(faceID);
			//printf("RemoveTriangle2 vertex [%d] adj face [%d] : \n", vertexID.Data(), faceID.Data());
		}
		else
		{
			polygon[index] = cd::VertexID::InvalidID;
		}
	}

	DisconnectAdjcentVertices(polygon[0], polygon[1]);
	DisconnectAdjcentVertices(polygon[1], polygon[2]);
	DisconnectAdjcentVertices(polygon[2], polygon[0]);
}

void ProgressiveMeshImpl::ReplaceVertexInFace(FaceID faceID, VertexID v0ID, VertexID v1ID)
{
	assert(faceID.IsValid());
	auto& face = GetFace(faceID.Data());
	assert(v0ID.IsValid());
	assert(v1ID.IsValid());
	assert(!face.GetVertexIDs().Contains(v1ID));
	auto optIndex = face.GetVertexIDs().GetIndex(v0ID);
	assert(optIndex.has_value());
	face.SetVertexID(static_cast<uint32_t>(optIndex.value()), v1ID);

	auto& v0 = GetVertex(v0ID.Data());
	auto& v1 = GetVertex(v1ID.Data());
	v0.GetAdjacentFaces().RemoveByValue(faceID);
	v1.GetAdjacentFaces().Add(faceID);

	for (int i = 0; i < 3; ++i)
	{
		auto& vertex = GetVertex(face.GetVertexID(i).Data());
		DisconnectAdjcentVertices(v0ID, vertex.GetID());
	}

	GetVertex(face.GetVertexID(0).Data()).AddAdjacentVertex(face.GetVertexID(1));
	GetVertex(face.GetVertexID(0).Data()).AddAdjacentVertex(face.GetVertexID(2));
	GetVertex(face.GetVertexID(1).Data()).AddAdjacentVertex(face.GetVertexID(0));
	GetVertex(face.GetVertexID(1).Data()).AddAdjacentVertex(face.GetVertexID(2));
	GetVertex(face.GetVertexID(2).Data()).AddAdjacentVertex(face.GetVertexID(0));
	GetVertex(face.GetVertexID(2).Data()).AddAdjacentVertex(face.GetVertexID(1));

	ComputeNormal(face);
}

void ProgressiveMeshImpl::ComputeNormal(cd::pm::Face& face)
{
	auto v0Index = face.GetVertexID(0U).Data();
	auto v1Index = face.GetVertexID(1U).Data();
	auto v2Index = face.GetVertexID(2U).Data();

	const auto& v0Position = GetVertex(v0Index).GetPosition();
	const auto& v1Position = GetVertex(v1Index).GetPosition();
	const auto& v2Position = GetVertex(v2Index).GetPosition();

	auto v1v0 = v1Position - v0Position;
	auto v2v0 = v2Position - v0Position;
	face.SetNormal(v1v0.Cross(v2v0).Normalize());
}

void ProgressiveMeshImpl::ComputeEdgeCollapseCostAtVertex(VertexID v0ID)
{
	uint32_t v0Index = v0ID.Data();
	auto& v0 = m_vertices[v0Index];
	if (v0.GetAdjacentVertices().Empty())
	{
		v0.SetCollapseCost(-1.0f);
		v0.SetCollapseTarget(cd::VertexID::InvalidID);
		return;
	}

	// Prevent not update after cost becomes 0 after initialization.
	v0.SetCollapseCost(FLT_MAX);
	v0.SetCollapseTarget(cd::VertexID::InvalidID);

	for (auto v1ID : v0.GetAdjacentVertices())
	{
		float cost = ComputeEdgeCollapseCostAtEdge(v0ID, v1ID);
		if (cost < v0.GetCollapseCost())
		{
			v0.SetCollapseCost(cost);
			v0.SetCollapseTarget(v1ID);
		}
	}
}

float ProgressiveMeshImpl::ComputeEdgeCollapseCostAtEdge(VertexID v0ID, VertexID v1ID)
{
	auto& v0 = m_vertices[v0ID.Data()];
	auto& v1 = m_vertices[v1ID.Data()];

	float edgeLength = (v0.GetPosition() - v1.GetPosition()).Length();
	float curvature = 0.0f;

	std::vector<FaceID> sideFaceIDs;
	for (auto v0Face : v0.GetAdjacentFaces())
	{
		if (GetFace(v0Face.Data()).GetVertexIDs().Contains(v1ID))
		{
			sideFaceIDs.push_back(v0Face);
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
			float t = (1.0f - normalDot) / 2.0f;
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

	float cost = edgeLength * curvature;
	return cost;
}

void ProgressiveMeshImpl::Collapse(VertexID v0ID, VertexID v1ID)
{
	if (!v1ID.IsValid())
	{
		RemoveVertex(v0ID);
		return;
	}

	auto& v0 = GetVertex(v0ID.Data());
	auto tmp = v0.GetAdjacentVertices();

	int adjFaceCount = static_cast<int>(v0.GetAdjacentFaces().Size());
	for (int i = adjFaceCount - 1; i >= 0; --i)
	{
		auto& face = GetFace(v0.GetAdjacentFaces().Get(i).Data());
		if (face.GetVertexIDs().Contains(v1ID))
		{
			//printf("\t2 Remove triangle [%d]\n", face.GetID().Data());
			RemoveFace(face.GetID());
		}
	}

	adjFaceCount = static_cast<int>(v0.GetAdjacentFaces().Size());
	for (int i = adjFaceCount - 1; i >= 0; --i)
	{
		//printf("\t2 Replace vertex [%d] -> [%d] for face [%d]\n", v0ID.Data(), v1ID.Data(), v0.GetAdjacentFaces()[i].Data());
		ReplaceVertexInFace(v0.GetAdjacentFaces()[i], v0ID, v1ID);
	}

	//printf("\t2 Remove vertex [%d]\n", v0ID.Data());
	RemoveVertex(v0ID);

	for (auto vertexID : tmp)
	{
		Vertex& v = GetVertex(vertexID.Data());
		m_minCostVertexQueue.erase(&v);
		//printf("\t2 ComputeEdgeCostAtVertex [%d]\n", vertexID.Data());
		ComputeEdgeCollapseCostAtVertex(vertexID);
		m_minCostVertexQueue.insert(&v);
	}
}

cd::Mesh ProgressiveMeshImpl::GenerateLodMesh(float percent, const cd::Mesh* pSourceMesh)
{
	assert(percent >= 0.0f && percent <= 1.0f);
	uint32_t targetFaceCount = static_cast<uint32_t>(GetFaceCount() * percent);
	return GenerateLodMesh(targetFaceCount, pSourceMesh);
}

cd::Mesh ProgressiveMeshImpl::GenerateLodMesh(float percent, uint32_t minFaceCount, const cd::Mesh* pSourceMesh)
{
	assert(percent >= 0.0f && percent <= 1.0f);
	uint32_t targetFaceCount = std::max(minFaceCount, static_cast<uint32_t>(GetFaceCount() * percent));
	return GenerateLodMesh(targetFaceCount, pSourceMesh);
}

cd::Mesh ProgressiveMeshImpl::GenerateLodMesh(uint32_t targetFaceCount, const cd::Mesh* pSourceMesh)
{
	auto collapseInfoPair = BuildCollapseOperations();
	std::vector<uint32_t> permutation = collapseInfoPair.first;
	std::vector<uint32_t> map = collapseInfoPair.second;

	uint32_t targetVertexCount = targetFaceCount * 3U;
	cd::Mesh mesh(targetVertexCount);

	if (pSourceMesh)
	{
		mesh.SetVertexFormat(pSourceMesh->GetVertexFormat());
	}
	else
	{
		mesh.GetVertexFormat().AddAttributeLayout(cd::VertexAttributeType::Position, cd::AttributeValueType::Float, 3);
	}

	const auto& vertexFormat = mesh.GetVertexFormat();
	for (uint32_t vertexIndex = 0U, totalVertexCount = GetVertexCount(); vertexIndex < totalVertexCount; ++vertexIndex)
	{
		uint32_t newVertexIndex = permutation[vertexIndex];
		assert(newVertexIndex < totalVertexCount);

		if (newVertexIndex < targetVertexCount)
		{
			// Vertex data can fetch from different data source :
			// 1. ProgressiveMesh's source mesh if it is still alive to query.
			// 2. Data stored in progressive mesh. Currently, only position is cached.
			if (vertexFormat.Contains(cd::VertexAttributeType::Position))
			{
				const auto& vertex = GetVertex(vertexIndex);
				mesh.SetVertexPosition(newVertexIndex, vertex.GetPosition());
			}

			if (pSourceMesh)
			{
				// SourceMesh vertex index should be same to initialized progressive mesh data.
				if (vertexFormat.Contains(cd::VertexAttributeType::Normal))
				{
					mesh.SetVertexNormal(newVertexIndex, pSourceMesh->GetVertexNormal(vertexIndex));
				}

				if (vertexFormat.Contains(cd::VertexAttributeType::Tangent))
				{
					mesh.SetVertexTangent(newVertexIndex, pSourceMesh->GetVertexTangent(vertexIndex));
				}

				if (vertexFormat.Contains(cd::VertexAttributeType::Bitangent))
				{
					mesh.SetVertexBiTangent(newVertexIndex, pSourceMesh->GetVertexBiTangent(vertexIndex));
				}

				if (vertexFormat.Contains(cd::VertexAttributeType::UV))
				{
					mesh.SetVertexUVSetCount(pSourceMesh->GetVertexUVSetCount());
					for (uint32_t setIndex = 0U; setIndex < mesh.GetVertexUVSetCount(); ++setIndex)
					{
						mesh.SetVertexUV(setIndex, newVertexIndex, pSourceMesh->GetVertexUV(setIndex, vertexIndex));
					}
				}

				if (vertexFormat.Contains(cd::VertexAttributeType::Color))
				{
					mesh.SetVertexColorSetCount(pSourceMesh->GetVertexColorSetCount());
					for (uint32_t setIndex = 0U; setIndex < mesh.GetVertexColorSetCount(); ++setIndex)
					{
						mesh.SetVertexColor(setIndex, newVertexIndex, pSourceMesh->GetVertexColor(setIndex, vertexIndex));
					}
				}

				if (vertexFormat.Contains(cd::VertexAttributeType::BoneIndex) && vertexFormat.Contains(cd::VertexAttributeType::BoneWeight))
				{
					mesh.SetVertexInfluenceCount(pSourceMesh->GetVertexInfluenceCount());
					for (uint32_t boneIndex = 0U; boneIndex < mesh.GetVertexInfluenceCount(); ++boneIndex)
					{
						mesh.SetVertexBoneWeight(boneIndex, newVertexIndex, pSourceMesh->GetVertexBoneID(boneIndex, vertexIndex), pSourceMesh->GetVertexWeight(boneIndex, vertexIndex));
					}
				}
			}
		}
	}

	cd::PolygonGroup polygonGroup;
	for (uint32_t faceIndex = 0U, totalFaceCount = GetFaceCount(); faceIndex < totalFaceCount; ++faceIndex)
	{
		const auto& face = GetFace(faceIndex);
		const auto& faceVertexIDs = face.GetVertexIDs();
		assert(faceVertexIDs.Size() == 3U);

		cd::Polygon newFace(3, VertexID::Invalid());
		for (uint32_t ii = 0U; ii < newFace.size(); ++ii)
		{
			uint32_t vertexIndex = faceVertexIDs[ii].Data();
			uint32_t newVertexIndex = permutation[vertexIndex];
			while (newVertexIndex > targetVertexCount)
			{
				newVertexIndex = map[newVertexIndex];
			}
			newFace[ii] = newVertexIndex;
		}

		if (newFace[0] == newFace[1] ||
			newFace[0] == newFace[2] ||
			newFace[1] == newFace[2])
		{
			continue;
		}

		polygonGroup.push_back(cd::MoveTemp(newFace));
		if (polygonGroup.size() >= targetFaceCount)
		{
			break;
		}
	}
	mesh.AddPolygonGroup(cd::MoveTemp(polygonGroup));

	return mesh;
}

}