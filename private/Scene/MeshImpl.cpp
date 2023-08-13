#include "MeshImpl.h"

namespace cd
{

void MeshImpl::FromHalfEdgeMesh(const hem::HalfEdgeMesh& halfEdgeMesh)
{
	SetVertexUVSetCount(1U);

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
			m_vertexPositions.emplace_back(h->GetVertex()->GetPosition());
			m_vertexNormals.emplace_back(h->GetCornerNormal());
			m_vertexUVSets[0].emplace_back(h->GetCornerUV());

			++vertexIndex;
			h = h->GetNext();
		} while (h != face.GetHalfEdge());

		uint32_t endVertexIndex = vertexIndex;
		for (uint32_t cornerIndex = beginVertexIndex + 1; cornerIndex < endVertexIndex - 1; ++cornerIndex)
		{
			m_polygons.emplace_back(cd::Polygon{beginVertexIndex, cornerIndex, cornerIndex + 1 });
		}
	}

	m_vertexCount = static_cast<uint32_t>(m_vertexPositions.size());
	m_polygonCount = static_cast<uint32_t>(m_polygons.size());
}

void MeshImpl::Init(uint32_t vertexCount, uint32_t polygonCount)
{
	m_vertexCount = vertexCount;
	m_polygonCount = polygonCount;

	assert(vertexCount > 0 && "No need to create an empty mesh.");
	assert(polygonCount > 0 && "Expect to generate index buffer by ourselves?");

	// TODO : You may get confused why use resize, not reserve.
	// The reason is that std::vector doesn't support operator[] access to read/write data if the real size not increases.
	// So it will be very convenient for binary stream read/write.
	// For example, you already get a byte stream and would like to use it to init Mesh's vertex buffers and index buffer.
	// You can't write data directly to std::vector as its size is 0.
	// The solution is to write a customized template dynamic array.
	m_vertexPositions.resize(vertexCount);
	m_vertexNormals.resize(vertexCount);
	m_vertexTangents.resize(vertexCount);
	m_vertexBiTangents.resize(vertexCount);

	m_polygons.resize(polygonCount);
}

////////////////////////////////////////////////////////////////////////////////////
// Vertex geometry data
////////////////////////////////////////////////////////////////////////////////////

void MeshImpl::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_vertexPositions[vertexIndex] = position;
}

void MeshImpl::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_vertexNormals[vertexIndex] = normal;
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

	// Create a vector of normals with the same size as the vertex positions vector,
	// initialized to the zero vector.
	std::vector<Direction> vertexNormals(vertexCount, Direction(0, 0, 0));

	// For each polygon in the mesh
	for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
	{
		const Polygon& polygon = GetPolygon(polygonIndex);

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

void MeshImpl::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_vertexTangents[vertexIndex] = tangent;
}

void MeshImpl::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_vertexBiTangents[vertexIndex] = biTangent;
}

void MeshImpl::ComputeVertexTangents()
{
	// Compute the tangents
	for (const auto& polygon : GetPolygons())
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
		m_vertexUVSets[setIndex].resize(m_vertexCount);
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
		m_vertexColorSets[setIndex].resize(m_vertexCount);
	}
}

void MeshImpl::SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color)
{
	m_vertexColorSets[setIndex][vertexIndex] = color;
}

////////////////////////////////////////////////////////////////////////////////////
// Vertex skin data
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::SetVertexInfluenceCount(uint32_t influenceCount)
{
	assert(m_vertexCount != 0U);

	m_vertexInfluenceCount = influenceCount;
	for (uint32_t influenceIndex = 0U; influenceIndex < influenceCount; ++influenceIndex)
	{
		m_vertexBoneIDs[influenceIndex].resize(m_vertexCount);
		m_vertexWeights[influenceIndex].resize(m_vertexCount);
	}
}

void MeshImpl::SetVertexBoneWeight(uint32_t boneIndex, uint32_t vertexIndex, BoneID boneID, VertexWeight weight)
{
	assert(vertexIndex < m_vertexCount);

	if(m_vertexBoneIDs[boneIndex].empty() &&
		m_vertexWeights[boneIndex].empty())
	{
		m_vertexBoneIDs[boneIndex].resize(m_vertexCount);
		m_vertexWeights[boneIndex].resize(m_vertexCount);

		++m_vertexInfluenceCount;

		assert(m_vertexInfluenceCount <= cd::MaxBoneInfluenceCount);
	}

	m_vertexBoneIDs[boneIndex][vertexIndex] = boneID;
	m_vertexWeights[boneIndex][vertexIndex] = weight;
}

////////////////////////////////////////////////////////////////////////////////////
// Polygon index data
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::SetPolygon(uint32_t polygonIndex, cd::Polygon polygon)
{
	m_polygons[polygonIndex] = MoveTemp(polygon);
}

cd::VertexID MeshImpl::GetPolygonVertexID(uint32_t polygonIndex, uint32_t vertexIndex) const
{
	return m_polygons[polygonIndex][vertexIndex];
}

}