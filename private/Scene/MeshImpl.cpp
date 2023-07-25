#include "MeshImpl.h"

#include <cassert>

namespace
{

template<typename T>
void SwapArrayElement(std::vector<T>& data, uint32_t v0, uint32_t v1)
{
	T temp = cd::MoveTemp(data[v0]);
	data[v0] = cd::MoveTemp(data[v1]);
	data[v1] = cd::MoveTemp(temp);
};

template<typename T>
void RemoveArrayElement(std::vector<T>& data, uint32_t v0)
{
	T temp = cd::MoveTemp(data.back());
	data[v0] = cd::MoveTemp(temp);
	data.pop_back();
};

}

namespace cd
{

MeshImpl::MeshImpl(uint32_t vertexCount, uint32_t polygonCount)
{
	Init(vertexCount, polygonCount);
}

MeshImpl::MeshImpl(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount) :
	m_id(meshID),
	m_name(MoveTemp(meshName))
{
	Init(vertexCount, polygonCount);
}

void MeshImpl::Init(uint32_t vertexCount, uint32_t polygonCount)
{
	m_vertexCount = vertexCount;
	m_polygonCount = polygonCount;

	assert(vertexCount > 0 && "No need to create an empty mesh.");
	assert(polygonCount > 0 && "Expect to generate index buffer by ourselves?");

	// pre-construct for attributes which almost all model files will have.
	m_vertexPositions.resize(vertexCount);
	m_vertexNormals.resize(vertexCount);
	m_vertexTangents.resize(vertexCount);
	m_vertexBiTangents.resize(vertexCount);

	m_polygons.resize(polygonCount);
}

void MeshImpl::Init(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_id = meshID;
	m_name = MoveTemp(meshName);
	Init(vertexCount, polygonCount);
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
// Vertex connectivity data
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::AddVertexAdjacentVertexID(uint32_t vertexIndex, VertexID vertexID)
{
	VertexIDArray& adjVertexArray = m_vertexAdjacentVertexArrays[vertexIndex];
	for (VertexID adjVertexID : adjVertexArray)
	{
		if (vertexID == adjVertexID)
		{
			return;
		}
	}

	adjVertexArray.push_back(vertexID);
}

void MeshImpl::AddVertexAdjacentPolygonID(uint32_t vertexIndex, PolygonID polygonID)
{
	PolygonIDArray& adjPolygonArray = m_vertexAdjacentPolygonArrays[vertexIndex];
	for (PolygonID adjPolygonID : adjPolygonArray)
	{
		if (polygonID == adjPolygonID)
		{
			return;
		}
	}

	m_vertexAdjacentPolygonArrays[vertexIndex].push_back(polygonID);
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


////////////////////////////////////////////////////////////////////////////////////
// Editing
////////////////////////////////////////////////////////////////////////////////////
void MeshImpl::MarkVertexInvalid(VertexID v)
{
	m_mapChaosVertexIDToIndex[v] = cd::VertexID::InvalidID;
}

bool MeshImpl::IsVertexValid(VertexID v) const
{
	auto itVertex = m_mapChaosVertexIDToIndex.find(v);
	return itVertex != m_mapChaosVertexIDToIndex.end() && itVertex->second == cd::VertexID::InvalidID;
}

//void MeshImpl::SwapVertex(VertexID v0, VertexID v1)
//{
//	m_mapChaosVertexIDToIndex[v0] = v1.Data();
//	m_mapChaosVertexIDToIndex[v1] = v0.Data();
//}

void MeshImpl::SwapVertexData(VertexID v0, VertexID v1)
{
	SwapArrayElement<Point>(m_vertexPositions, v0.Data(), v1.Data());
	SwapArrayElement<Direction>(m_vertexNormals, v0.Data(), v1.Data());
	SwapArrayElement<Direction>(m_vertexTangents, v0.Data(), v1.Data());
	SwapArrayElement<Direction>(m_vertexBiTangents, v0.Data(), v1.Data());

	for (uint32_t m_colorSetIndex = 0; m_colorSetIndex < m_vertexColorSetCount; ++m_colorSetIndex)
	{
		SwapArrayElement<Color>(m_vertexColorSets[m_colorSetIndex], v0.Data(), v1.Data());
	}

	for (uint32_t m_uvSetIndex = 0; m_uvSetIndex < m_vertexUVSetCount; ++m_uvSetIndex)
	{
		SwapArrayElement<UV>(m_vertexUVSets[m_uvSetIndex], v0.Data(), v1.Data());
	}

	for (uint32_t m_influenceIndex = 0; m_influenceIndex < m_vertexInfluenceCount; ++m_influenceIndex)
	{
		SwapArrayElement<BoneID>(m_vertexBoneIDs[m_influenceIndex], v0.Data(), v1.Data());
		SwapArrayElement<VertexWeight>(m_vertexWeights[m_influenceIndex], v0.Data(), v1.Data());
	}

	SwapArrayElement<VertexIDArray>(m_vertexAdjacentVertexArrays, v0.Data(), v1.Data());
	SwapArrayElement<PolygonIDArray>(m_vertexAdjacentPolygonArrays, v0.Data(), v1.Data());
}

void MeshImpl::RemoveVertexData(VertexID v0)
{
	RemoveArrayElement<Point>(m_vertexPositions, v0.Data());
	RemoveArrayElement<Direction>(m_vertexNormals, v0.Data());
	RemoveArrayElement<Direction>(m_vertexTangents, v0.Data());
	RemoveArrayElement<Direction>(m_vertexBiTangents, v0.Data());

	for (uint32_t m_colorSetIndex = 0; m_colorSetIndex < m_vertexColorSetCount; ++m_colorSetIndex)
	{
		RemoveArrayElement<Color>(m_vertexColorSets[m_colorSetIndex], v0.Data());
	}

	for (uint32_t m_uvSetIndex = 0; m_uvSetIndex < m_vertexUVSetCount; ++m_uvSetIndex)
	{
		RemoveArrayElement<UV>(m_vertexUVSets[m_uvSetIndex], v0.Data());
	}

	for (uint32_t m_influenceIndex = 0; m_influenceIndex < m_vertexInfluenceCount; ++m_influenceIndex)
	{
		RemoveArrayElement<BoneID>(m_vertexBoneIDs[m_influenceIndex], v0.Data());
		RemoveArrayElement<VertexWeight>(m_vertexWeights[m_influenceIndex], v0.Data());
	}

	RemoveArrayElement<VertexIDArray>(m_vertexAdjacentVertexArrays, v0.Data());
	RemoveArrayElement<PolygonIDArray>(m_vertexAdjacentPolygonArrays, v0.Data());

	--m_vertexCount;
}

void MeshImpl::MarkPolygonInvalid(PolygonID p)
{
	m_mapChaosPolygonIDToIndex[p] = cd::VertexID::InvalidID;
}

bool MeshImpl::IsPolygonValid(PolygonID p) const
{
	auto itPolygon = m_mapChaosPolygonIDToIndex.find(p);
	return itPolygon != m_mapChaosPolygonIDToIndex.end() && itPolygon->second == cd::PolygonID::InvalidID;
}

void MeshImpl::RemovePolygonData(PolygonID p)
{
	RemoveArrayElement<Polygon>(m_polygons, p.Data());

	--m_polygonCount;
}

void MeshImpl::Unify()
{
	for (const auto& [vertexID, vertexIndex] : m_mapChaosVertexIDToIndex)
	{
		if (cd::VertexID::InvalidID == vertexIndex)
		{
			RemoveVertexData(vertexID);
			m_mapChaosVertexIDToIndex.erase(vertexID);
		}
	}

	for (const auto& [polygonID, polygonIndex] : m_mapChaosPolygonIDToIndex)
	{
		if (cd::VertexID::InvalidID == polygonIndex)
		{
			RemovePolygonData(polygonID);
			m_mapChaosPolygonIDToIndex.erase(polygonID);
		}
	}
}

}