#include "Math/MeshGenerator.h"

#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Scene/Mesh.h"
#include "Scene/VertexFormat.h"

#include <cassert>

namespace cd
{

std::optional<Mesh> MeshGenerator::Generate(const Box& box, const VertexFormat& vertexFormat)
{
	assert(vertexFormat.Contains(VertexAttributeType::Position));

	const Vec3f& min = box.Min();
	const Vec3f& max = box.Max();
	Vec3f extent = max - min;

	//          0 --- 1 (max)
	//        / |   / |
	//       4 --- 5  |
	//       |  |  |  |
	//       |  2 -|- 3
	//       | /   | /
	// (min) 6 --- 7

	Mesh mesh(8U, 12U);

	// Vertex positions
	mesh.SetVertexPosition(0U, Point(min.x(), max.y(), max.z()));
	mesh.SetVertexPosition(1U, Point(max.x(), max.y(), max.z()));
	mesh.SetVertexPosition(2U, Point(min.x(), min.y(), max.z()));
	mesh.SetVertexPosition(3U, Point(max.x(), min.y(), max.z()));
	mesh.SetVertexPosition(4U, Point(min.x(), max.y(), min.z()));
	mesh.SetVertexPosition(5U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(6U, Point(min.x(), min.y(), min.z()));
	mesh.SetVertexPosition(7U, Point(max.x(), min.y(), min.z()));

	// Index buffer
	mesh.SetPolygon(0U, VertexID(0U), VertexID(1U), VertexID(2U));
	mesh.SetPolygon(1U, VertexID(1U), VertexID(3U), VertexID(2U));
	mesh.SetPolygon(2U, VertexID(4U), VertexID(6U), VertexID(5U));
	mesh.SetPolygon(3U, VertexID(5U), VertexID(6U), VertexID(7U));
	mesh.SetPolygon(4U, VertexID(0U), VertexID(2U), VertexID(4U));
	mesh.SetPolygon(5U, VertexID(4U), VertexID(2U), VertexID(6U));
	mesh.SetPolygon(6U, VertexID(1U), VertexID(5U), VertexID(3U));
	mesh.SetPolygon(7U, VertexID(5U), VertexID(7U), VertexID(3U));
	mesh.SetPolygon(8U, VertexID(0U), VertexID(4U), VertexID(1U));
	mesh.SetPolygon(9U, VertexID(4U), VertexID(5U), VertexID(1U));
	mesh.SetPolygon(10U, VertexID(2U), VertexID(3U), VertexID(6U));
	mesh.SetPolygon(11U, VertexID(6U), VertexID(3U), VertexID(7U));

	//if (vertexFormat.Contains(VertexAttributeType::Normal))
	//{
	//	for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
	//	{
	//		mesh.SetVertexNormal(vertexIndex, Direction(0.0f));
	//	}
	//
	//	for (uint32_t polygonIndex = 0U; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
	//	{
	//		const Mesh::Polygon& polygon = mesh.GetPolygon(polygonIndex);
	//
	//		const Point& v1 = mesh.GetVertexPosition(polygon[0].Data());
	//		const Point& v2 = mesh.GetVertexPosition(polygon[1].Data());
	//		const Point& v3 = mesh.GetVertexPosition(polygon[2].Data());
	//
	//		Direction faceNormal = (v2 - v1).Cross(v3 - v1);
	//		for (uint32_t pointIndex = 0U; pointIndex < 3U; ++pointIndex)
	//		{
	//			uint32_t vertexIndex = polygon[pointIndex].Data();
	//			Direction& vertexNormal = mesh.GetVertexNormal(vertexIndex);
	//			vertexNormal += faceNormal;
	//		}
	//	}
	//
	//	for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
	//	{
	//		Direction& vertexNormal = mesh.GetVertexNormal(vertexIndex);
	//		vertexNormal.Normalize();
	//	}
	//}

	return mesh;
}

std::optional<Mesh> MeshGenerator::GenerateNonUnique(const Box& box, const VertexFormat& vertexFormat)
{
	assert(vertexFormat.Contains(VertexAttributeType::Position));

	const Vec3f& min = box.Min();
	const Vec3f& max = box.Max();
	Vec3f extent = max - min;

	//          0 --- 1 (max)
	//        / |   / |
	//       4 --- 5  |
	//       |  |  |  |
	//       |  2 -|- 3
	//       | /   | /
	// (min) 6 --- 7

	Mesh mesh(36U, 12U);

	// Vertex positions
	// 0 - 1 - 3 - 2
	mesh.SetVertexPosition(0U, Point(min.x(), max.y(), max.z()));
	mesh.SetVertexPosition(1U, Point(max.x(), max.y(), max.z()));
	mesh.SetVertexPosition(2U, Point(min.x(), min.y(), max.z()));
	mesh.SetVertexPosition(3U, Point(max.x(), max.y(), max.z()));
	mesh.SetVertexPosition(4U, Point(max.x(), min.y(), max.z()));
	mesh.SetVertexPosition(5U, Point(min.x(), min.y(), max.z()));

	// 5 - 1 - 3 - 7
	mesh.SetVertexPosition(6U, Point(max.x(), max.y(), max.z()));
	mesh.SetVertexPosition(7U, Point(max.x(), min.y(), max.z()));
	mesh.SetVertexPosition(8U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(9U, Point(max.x(), min.y(), max.z()));
	mesh.SetVertexPosition(10U, Point(max.x(), min.y(), min.z()));
	mesh.SetVertexPosition(11U, Point(max.x(), max.y(), min.z()));

	// 4 - 5 - 7 - 6
	mesh.SetVertexPosition(12U, Point(min.x(), max.y(), min.z()));
	mesh.SetVertexPosition(13U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(14U, Point(min.x(), min.y(), min.z()));
	mesh.SetVertexPosition(15U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(16U, Point(max.x(), min.y(), min.z()));
	mesh.SetVertexPosition(17U, Point(min.x(), min.y(), min.z()));

	// 0 - 4 - 6 - 2
	mesh.SetVertexPosition(18U, Point(min.x(), max.y(), max.z()));
	mesh.SetVertexPosition(19U, Point(min.x(), max.y(), min.z()));
	mesh.SetVertexPosition(20U, Point(min.x(), min.y(), max.z()));
	mesh.SetVertexPosition(21U, Point(min.x(), max.y(), min.z()));
	mesh.SetVertexPosition(22U, Point(min.x(), min.y(), min.z()));
	mesh.SetVertexPosition(23U, Point(min.x(), min.y(), max.z()));

	// 0 - 1 - 5 - 4
	mesh.SetVertexPosition(24U, Point(min.x(), max.y(), max.z()));
	mesh.SetVertexPosition(25U, Point(max.x(), max.y(), max.z()));
	mesh.SetVertexPosition(26U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(27U, Point(min.x(), max.y(), max.z()));
	mesh.SetVertexPosition(28U, Point(max.x(), max.y(), min.z()));
	mesh.SetVertexPosition(29U, Point(min.x(), max.y(), min.z()));

	// 2 - 3 - 7 - 6
	mesh.SetVertexPosition(30U, Point(min.x(), min.y(), max.z()));
	mesh.SetVertexPosition(31U, Point(max.x(), min.y(), max.z()));
	mesh.SetVertexPosition(32U, Point(max.x(), min.y(), min.z()));
	mesh.SetVertexPosition(33U, Point(min.x(), min.y(), max.z()));
	mesh.SetVertexPosition(34U, Point(max.x(), min.y(), min.z()));
	mesh.SetVertexPosition(35U, Point(min.x(), min.y(), min.z()));
	
	// TODO : used by braycentric coordinates.
	if (vertexFormat.Contains(VertexAttributeType::Bitangent))
	{
		uint32_t vertexIndex = 0U;
		for (uint32_t polygonIndex = 0U; polygonIndex < 12U; ++polygonIndex)
		{
			uint32_t aIndex = vertexIndex++;
			uint32_t bIndex = vertexIndex++;
			uint32_t cIndex = vertexIndex++;
			mesh.SetVertexBiTangent(aIndex, cd::Vec3f(1.0f, 0.0f, 0.0f));
			mesh.SetVertexBiTangent(bIndex, cd::Vec3f(0.0f, 1.0f, 0.0f));
			mesh.SetVertexBiTangent(cIndex, cd::Vec3f(0.0f, 0.0f, 1.0f));
		}
	}

	// Index buffer
	uint32_t vertexIndex = 0U;
	for (uint32_t polygonIndex = 0U; polygonIndex < 12U; ++polygonIndex)
	{
		VertexID v0(vertexIndex++);
		VertexID v1(vertexIndex++);
		VertexID v2(vertexIndex++);
		mesh.SetPolygon(polygonIndex, v0, v1, v2);
	}

	return mesh;
}

std::optional<Mesh> MeshGenerator::Generate(const Plane& plane, float width, float height, const VertexFormat& vertexFormat)
{
	assert(vertexFormat.Contains(VertexAttributeType::Position) && "Don't want to generate position?");

	//const Vec3f& normal = plane.GetNormal();
	//Vec3f origin = plane.GetOrigin();
	//
	//// 0 --- 1
	//// |  \  |
	//// 2 --- 3
	//
	//Mesh mesh(4U, 2U);
	//mesh.SetVertexPosition(0U, Point(min.x(), max.y(), min.z()));
	//mesh.SetVertexPosition(1U, Point(min.x(), min.y(), max.z()));
	//mesh.SetVertexPosition(2U, Point(max.x(), min.y(), max.z()));
	//mesh.SetVertexPosition(3U, Point(max.x(), min.y(), min.z()));
	//
	//if (vertexFormat.Contains(VertexAttributeType::Normal))
	//{
	//	mesh.SetVertexNormal(0U, Direction(0.0f, 1.0f, 0.0f));
	//	mesh.SetVertexNormal(1U, Direction(0.0f, 1.0f, 0.0f));
	//	mesh.SetVertexNormal(2U, Direction(0.0f, 1.0f, 0.0f));
	//	mesh.SetVertexNormal(3U, Direction(0.0f, 1.0f, 0.0f));
	//}
	//
	//if (vertexFormat.Contains(VertexAttributeType::UV))
	//{
	//	mesh.SetVertexUVSetCount(1);
	//	mesh.SetVertexUV(0U, 0U, UV(0.0f, 0.0f));
	//	mesh.SetVertexUV(0U, 1U, UV(1.0f, 0.0f));
	//	mesh.SetVertexUV(0U, 2U, UV(1.0f, 1.0f));
	//	mesh.SetVertexUV(0U, 3U, UV(0.0f, 1.0f));
	//}
	//
	//mesh.SetPolygon(0U, VertexID(0U), VertexID(1U), VertexID(3U));
	//mesh.SetPolygon(1U, VertexID(0U), VertexID(3U), VertexID(2U));
	
	return std::nullopt;
}

std::optional<Mesh> MeshGenerator::Generate(const Sphere& sphere, const VertexFormat& vertexFormat)
{
	return std::nullopt;
}

}