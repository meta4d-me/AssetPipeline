#include "MeshImpl.h"

#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/Vertex.h"
#include "Hashers/HashCombine.hpp"

#include <cfloat>

namespace cd
{

void MeshImpl::FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy)
{
	m_vertexUVSetCount = 1U;

	auto& vertexPositions = GetVertexPositions();
	auto& vertexNormals = GetVertexNormals();

	cd::PolygonGroup polygonGroup;
	if (ConvertStrategy::ShadingFirst == strategy)
	{
		uint32_t vertexIndex = 0U;
		for (const auto& face : halfEdgeMesh.GetFaces())
		{
			if (face.IsBoundary())
			{
				continue;
			}

			uint32_t beginVertexIndex = vertexIndex;
			hem::HalfEdgeCRef h = face.GetHalfEdge();
			do
			{
				vertexPositions.emplace_back(h->GetVertex()->GetPosition());
				vertexNormals.emplace_back(h->GetCornerNormal());
				m_vertexUVSets[0].emplace_back(h->GetCornerUV());

				++vertexIndex;
				h = h->GetNext();
			} while (h != face.GetHalfEdge());

			uint32_t endVertexIndex = vertexIndex;
			for (uint32_t cornerIndex = beginVertexIndex + 1; cornerIndex < endVertexIndex - 1; ++cornerIndex)
			{
				polygonGroup.emplace_back(cd::Polygon{beginVertexIndex, cornerIndex, cornerIndex + 1 });
			}
		}
	}
	else if (ConvertStrategy::TopologyFirst == strategy)
	{
		std::unordered_map<hem::VertexCRef, uint32_t> vertexRefToIndex;

		const auto& vertices = halfEdgeMesh.GetVertices();
		for (hem::VertexCRef vertex = vertices.begin(); vertex != vertices.end(); ++vertex)
		{
			vertexPositions.emplace_back(vertex->GetPosition());

			// Fill normal/uv data later by looping through half edges.
			vertexNormals.emplace_back(0.0f);
			m_vertexUVSets[0].emplace_back(0.0f);

			auto result = vertexRefToIndex.emplace(vertex, static_cast<uint32_t>(vertexPositions.size() - 1));
			assert(result.second); // Make sure it is unique.
		}

		std::vector<uint32_t> cornerCountInVertex;
		cornerCountInVertex.resize(vertexRefToIndex.size(), 0U);

		for (const auto& face : halfEdgeMesh.GetFaces())
		{
			if (face.IsBoundary())
			{
				continue;
			}

			std::vector<uint32_t> faceIndexes;
			hem::HalfEdgeCRef h = face.GetHalfEdge();
			do
			{
				auto itIndex = vertexRefToIndex.find(h->GetVertex());
				assert(itIndex != vertexRefToIndex.end());

				uint32_t vertexIndex = itIndex->second;
				faceIndexes.emplace_back(vertexIndex);

				// Add corners' normal/uv data to previously created vertex.
				vertexNormals[vertexIndex] += h->GetCornerNormal();
				m_vertexUVSets[0][vertexIndex] += h->GetCornerUV();
				cornerCountInVertex[vertexIndex] += 1U;

				h = h->GetNext();
			} while (h != face.GetHalfEdge());

			assert(faceIndexes.size() >= 3);
			for (uint32_t cornerIndex = 1; cornerIndex < faceIndexes.size() - 1; ++cornerIndex)
			{
				polygonGroup.emplace_back(cd::Polygon{faceIndexes[0], faceIndexes[cornerIndex], faceIndexes[cornerIndex + 1]});
			}
		}

		// Average normal/uv data per vertex.
		for (uint32_t vertexIndex = 0U; vertexIndex < cornerCountInVertex.size(); ++vertexIndex)
		{
			uint32_t cornerCount = cornerCountInVertex[vertexIndex];
			if (cornerCount > 1U)
			{
				vertexNormals[vertexIndex].Normalize();
				m_vertexUVSets[0][vertexIndex] /= static_cast<float>(cornerCount);
			}
		}
	}
	else if (ConvertStrategy::BoundaryOnly == strategy)
	{
		uint32_t vertexIndex = 0U;
		for (const auto& face : halfEdgeMesh.GetFaces())
		{
			if (!face.IsBoundary())
			{
				continue;
			}

			uint32_t beginVertexIndex = vertexIndex;
			hem::HalfEdgeCRef h = face.GetHalfEdge();
			do
			{
				vertexPositions.emplace_back(h->GetVertex()->GetPosition());
				vertexNormals.emplace_back(h->GetCornerNormal());
				m_vertexUVSets[0].emplace_back(h->GetCornerUV());

				++vertexIndex;
				h = h->GetNext();
			} while (h != face.GetHalfEdge());

			uint32_t endVertexIndex = vertexIndex;
			cd::Polygon polygon;
			polygon.reserve(endVertexIndex);
			for (uint32_t index = beginVertexIndex; index < endVertexIndex; ++index)
			{
				polygon.emplace_back(index);
			}

			polygonGroup.emplace_back(cd::MoveTemp(polygon));
		}
	}
	else
	{
		assert("Unsupported convert strategy.");
	}

	GetPolygonGroups().emplace_back(cd::MoveTemp(polygonGroup));

	// Make capcity same with actual size.
	ShrinkToFit();
}

void MeshImpl::Init(uint32_t vertexCount)
{
	SetVertexPositionCount(vertexCount);
	SetVertexInstanceIDCount(0U);

	InitVertexAttributes(vertexCount);
}

void MeshImpl::Init(uint32_t vertexPositionCount, uint32_t vertexAttributeCount)
{
	SetVertexPositionCount(vertexPositionCount);
	SetVertexInstanceIDCount(vertexPositionCount);

	InitVertexAttributes(vertexAttributeCount);
}

void MeshImpl::InitVertexAttributes(uint32_t vertexAttributeCount)
{
	SetVertexAttributeCount(vertexAttributeCount);
	SetVertexNormalCount(GetVertexAttributeCount());
	SetVertexTangentCount(GetVertexAttributeCount());
	SetVertexBiTangentCount(GetVertexAttributeCount());

	for (uint32_t setIndex = 0U; setIndex < GetVertexUVSetCount(); ++setIndex)
	{
		m_vertexUVSets[setIndex].resize(GetVertexAttributeCount());
	}

	for (uint32_t setIndex = 0U; setIndex < m_vertexColorSetCount; ++setIndex)
	{
		m_vertexColorSets[setIndex].resize(GetVertexAttributeCount());
	}
}

void MeshImpl::ShrinkToFit()
{
	GetVertexPositions().shrink_to_fit();
	GetVertexInstanceIDs().shrink_to_fit();

	GetVertexNormals().shrink_to_fit();
	GetVertexTangents().shrink_to_fit();
	GetVertexBiTangents().shrink_to_fit();
	for (uint32_t setIndex = 0U; setIndex < GetVertexUVSetCount(); ++setIndex)
	{
		m_vertexUVSets[setIndex].shrink_to_fit();
	}
	for (uint32_t setIndex = 0U; setIndex < GetVertexColorSetCount(); ++setIndex)
	{
		m_vertexColorSets[setIndex].shrink_to_fit();
	}

	GetPolygonGroups().shrink_to_fit();
	for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < GetPolygonGroupCount(); ++polygonGroupIndex)
	{
		GetPolygonGroup(polygonGroupIndex).shrink_to_fit();
	}
}

uint32_t MeshImpl::GetPolygonCount() const
{
	size_t polygonCount = 0U;
	for (const auto& polygonGroup : GetPolygonGroups())
	{
		polygonCount += polygonGroup.size();
	}

	return static_cast<uint32_t>(polygonCount);
}

////////////////////////////////////////////////////////////////////////////////////
// Vertex geometry data
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::UpdateAABB()
{
	cd::Point minPoint(FLT_MAX);
	cd::Point maxPoint(-FLT_MAX);
	for (const cd::Point& position : GetVertexPositions())
	{
		minPoint.x() = minPoint.x() > position.x() ? position.x() : minPoint.x();
		minPoint.y() = minPoint.y() > position.y() ? position.y() : minPoint.y();
		minPoint.z() = minPoint.z() > position.z() ? position.z() : minPoint.z();
		maxPoint.x() = maxPoint.x() > position.x() ? maxPoint.x() : position.x();
		maxPoint.y() = maxPoint.y() > position.y() ? maxPoint.y() : position.y();
		maxPoint.z() = maxPoint.z() > position.z() ? maxPoint.z() : position.z();
	}

	SetAABB(cd::AABB(cd::MoveTemp(minPoint), cd::MoveTemp(maxPoint)));
}

void MeshImpl::ComputeVertexNormals()
{
	const uint32_t vertexCount = GetVertexCount();
	const uint32_t polygonCount = GetPolygonCount();

	if (vertexCount == 0U || polygonCount == 0U)
	{
		// Cannot compute normals without vertex positions or polygons
		return;
	}

	SetVertexNormalCount(vertexCount);

	// Create a vector of normals with the same size as the vertex positions vector,
	// initialized to the zero vector.
	std::vector<Direction> vertexNormals(vertexCount, Direction(0, 0, 0));

	// For each polygon in the mesh
	for (const auto& polygonGroup : GetPolygonGroups())
	{
		for (const auto& polygon : polygonGroup)
		{
			// Calculate the polygon normal
			const Point& p0 = GetVertexPosition(polygon[0].Data());
			const Point& p1 = GetVertexPosition(polygon[1].Data());
			const Point& p2 = GetVertexPosition(polygon[2].Data());
			const Direction polygonNormal = (p1 - p0).Cross(p2 - p0).Normalize();

			// Add the polygon normal to the normal of each of its vertices
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3; ++polygonVertexIndex)
			{
				const uint32_t vertexIndex = polygon[polygonVertexIndex].Data();
				vertexNormals[vertexIndex] += polygonNormal;
			}
		}
	}

	// Normalize all vertex normals
	for (Direction& normal : vertexNormals)
	{
		normal.Normalize();
	}

	// Set the computed normals back to the mesh
	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		SetVertexNormal(i, vertexNormals[i]);
	}
}

void MeshImpl::ComputeVertexTangents()
{
	SetVertexTangentCount(GetVertexCount());

	// Compute the tangents
	for (const auto& polygonGroup : GetPolygonGroups())
	{
		for (const auto& polygon : polygonGroup)
		{
			const auto& position1 = GetVertexPosition(polygon[0].Data());
			const auto& position2 = GetVertexPosition(polygon[1].Data());
			const auto& position3 = GetVertexPosition(polygon[2].Data());

			const auto& uv1 = GetVertexUV(0U, polygon[0].Data());
			const auto& uv2 = GetVertexUV(0U, polygon[1].Data());
			const auto& uv3 = GetVertexUV(0U, polygon[2].Data());

			const cd::Vec3f edge1(position2.x() - position1.x(), position2.y() - position1.y(), position2.z() - position1.z());
			const cd::Vec3f edge2(position3.x() - position1.x(), position3.y() - position1.y(), position3.z() - position1.z());

			const float deltaU1 = uv2.x() - uv1.x();
			const float deltaV1 = uv2.y() - uv1.y();
			const float deltaU2 = uv3.x() - uv1.x();
			const float deltaV2 = uv3.y() - uv1.y();

			const float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			Direction tangent;
			tangent.x() = f * (deltaV2 * edge1.x() - deltaV1 * edge2.x());
			tangent.y() = f * (deltaV2 * edge1.y() - deltaV1 * edge2.y());
			tangent.z() = f * (deltaV2 * edge1.z() - deltaV1 * edge2.z());

			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3; ++polygonVertexIndex)
			{
				GetVertexTangent(polygon[polygonVertexIndex].Data()) += tangent;
			}
		}
	}

	// Normalize the tangents
	for (auto& tangent : GetVertexTangents())
	{
		tangent.Normalize();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Vertex texturing data
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::SetVertexUVSetCount(uint32_t setCount)
{
	m_vertexUVSetCount = setCount;
	for(uint32_t setIndex = 0U; setIndex < m_vertexUVSetCount; ++setIndex)
	{
		m_vertexUVSets[setIndex].resize(GetVertexAttributeCount());
	}
}

void MeshImpl::SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv)
{
	m_vertexUVSets[setIndex][vertexIndex] = uv;
}

void MeshImpl::SetVertexColorSetCount(uint32_t setCount)
{
	m_vertexColorSetCount = setCount;
	for (uint32_t setIndex = 0U; setIndex < m_vertexColorSetCount; ++setIndex)
	{
		m_vertexColorSets[setIndex].resize(GetVertexAttributeCount());
	}
}

void MeshImpl::SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color)
{
	m_vertexColorSets[setIndex][vertexIndex] = color;
}

}