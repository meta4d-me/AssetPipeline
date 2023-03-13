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

	// Index buffer
	mesh.SetPolygon(0U, VertexID(0U), VertexID(1U), VertexID(2U));
	mesh.SetPolygon(1U, VertexID(1U), VertexID(3U), VertexID(2U));
	mesh.SetPolygon(2U, VertexID(4U), VertexID(6U), VertexID(5U));
	mesh.SetPolygon(3U, VertexID(5U), VertexID(6U), VertexID(7U));
	mesh.SetPolygon(4U, VertexID(0U), VertexID(2U), VertexID(6U));
	mesh.SetPolygon(5U, VertexID(4U), VertexID(0U), VertexID(6U));
	mesh.SetPolygon(6U, VertexID(1U), VertexID(3U), VertexID(7U));
	mesh.SetPolygon(7U, VertexID(5U), VertexID(1U), VertexID(7U));
	mesh.SetPolygon(8U, VertexID(0U), VertexID(4U), VertexID(1U));
	mesh.SetPolygon(9U, VertexID(4U), VertexID(5U), VertexID(1U));
	mesh.SetPolygon(10U, VertexID(2U), VertexID(3U), VertexID(6U));
	mesh.SetPolygon(11U, VertexID(6U), VertexID(3U), VertexID(7U));

	cd::VertexFormat meshVertexFormat;

	// Vertex positions
	if (vertexFormat.Contains(VertexAttributeType::Position))
	{
		mesh.SetVertexPosition(0U, Point(min.x(), max.y(), max.z()));
		mesh.SetVertexPosition(1U, Point(max.x(), max.y(), max.z()));
		mesh.SetVertexPosition(2U, Point(min.x(), min.y(), max.z()));
		mesh.SetVertexPosition(3U, Point(max.x(), min.y(), max.z()));
		mesh.SetVertexPosition(4U, Point(min.x(), max.y(), min.z()));
		mesh.SetVertexPosition(5U, Point(max.x(), max.y(), min.z()));
		mesh.SetVertexPosition(6U, Point(min.x(), min.y(), min.z()));
		mesh.SetVertexPosition(7U, Point(max.x(), min.y(), min.z()));
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::Normal))
	{
		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			mesh.SetVertexNormal(vertexIndex, Direction(0.0f, 1.0f, 0.0f));
		}

		//for (uint32_t polygonIndex = 0U; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
		//{
		//	const Polygon& polygon = mesh.GetPolygon(polygonIndex);
		//
		//	const Point& v1 = mesh.GetVertexPosition(polygon[0].Data());
		//	const Point& v2 = mesh.GetVertexPosition(polygon[1].Data());
		//	const Point& v3 = mesh.GetVertexPosition(polygon[2].Data());
		//
		//	Direction faceNormal = (v2 - v1).Cross(v3 - v1);
		//	for (uint32_t pointIndex = 0U; pointIndex < 3U; ++pointIndex)
		//	{
		//		uint32_t vertexIndex = polygon[pointIndex].Data();
		//		Direction& vertexNormal = mesh.GetVertexNormal(vertexIndex);
		//		vertexNormal += faceNormal;
		//	}
		//}
		//
		//for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		//{
		//	Direction& vertexNormal = mesh.GetVertexNormal(vertexIndex);
		//	vertexNormal.Normalize();
		//}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	// TODO : used by braycentric coordinates.
	if (vertexFormat.Contains(VertexAttributeType::Bitangent))
	{
		mesh.SetVertexBiTangent(0U, cd::Vec3f(1.0f, 0.0f, 0.0f));
		mesh.SetVertexBiTangent(1U, cd::Vec3f(0.0f, 1.0f, 0.0f));
		mesh.SetVertexBiTangent(2U, cd::Vec3f(0.0f, 0.0f, 1.0f));
		mesh.SetVertexBiTangent(3U, cd::Vec3f(1.0f, 0.0f, 0.0f));
		mesh.SetVertexBiTangent(4U, cd::Vec3f(0.0f, 0.0f, 1.0f));
		mesh.SetVertexBiTangent(5U, cd::Vec3f(1.0f, 0.0f, 0.0f));
		mesh.SetVertexBiTangent(6U, cd::Vec3f(0.0f, 1.0f, 0.0f));
		mesh.SetVertexBiTangent(7U, cd::Vec3f(0.0f, 0.0f, 1.0f));
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::Tangent))
	{
		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			mesh.SetVertexTangent(vertexIndex, Direction(1.0f, 0.0f, 0.0f));
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::UV))
	{
		mesh.SetVertexUVSetCount(1);
		mesh.SetVertexUV(0U, 0U, cd::UV(0.0f, 0.0f));
		mesh.SetVertexUV(0U, 1U, cd::UV(0.0f, 1.0f));
		mesh.SetVertexUV(0U, 2U, cd::UV(1.0f, 0.0f));
		mesh.SetVertexUV(0U, 3U, cd::UV(1.0f, 1.0f));
		mesh.SetVertexUV(0U, 4U, cd::UV(0.0f, 0.0f));
		mesh.SetVertexUV(0U, 5U, cd::UV(0.0f, 1.0f));
		mesh.SetVertexUV(0U, 6U, cd::UV(1.0f, 0.0f));
		mesh.SetVertexUV(0U, 7U, cd::UV(1.0f, 1.0f));
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}

	mesh.SetVertexFormat(MoveTemp(meshVertexFormat));
	mesh.SetAABB(AABB(min, max));

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