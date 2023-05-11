#include "Math/MeshGenerator.h"

#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Scene/Mesh.h"
#include "Scene/VertexFormat.h"

#include <array>
#include <cassert>

namespace cd
{

std::optional<Mesh> MeshGenerator::Generate(const Box& box, const VertexFormat& vertexFormat, bool useCounterWiseForFrontFace)
{
	assert(vertexFormat.Contains(VertexAttributeType::Position));

	const auto& min = box.Min();
	const auto& max = box.Max();
	const auto extent = max - min;

	//          0 --- 1 (max)
	//        / |   / |
	//       4 --- 5  |
	//       |  |  |  |
	//       |  2 -|- 3
	//       | /   | /
	// (min) 6 --- 7

	std::array<cd::Point, 8> positions = {
		cd::Point(min.x(), max.y(), max.z()),
		cd::Point(max.x(), max.y(), max.z()),
		cd::Point(min.x(), min.y(), max.z()),
		cd::Point(max.x(), min.y(), max.z()),
		cd::Point(min.x(), max.y(), min.z()),
		cd::Point(max.x(), max.y(), min.z()),
		cd::Point(min.x(), min.y(), min.z()),
		cd::Point(max.x(), min.y(), min.z())
	};

	std::array<cd::Polygon, 12> polygons = {
		cd::Polygon{0U, 2U, 1U},
		cd::Polygon{1U, 2U, 3U},
		cd::Polygon{4U, 5U, 6U},
		cd::Polygon{5U, 7U, 6U},
		cd::Polygon{0U, 6U, 2U},
		cd::Polygon{4U, 6U, 0U},
		cd::Polygon{1U, 3U, 7U},
		cd::Polygon{5U, 1U, 7U},
		cd::Polygon{0U, 1U, 4U},
		cd::Polygon{4U, 1U, 5U},
		cd::Polygon{2U, 6U, 3U},
		cd::Polygon{6U, 7U, 3U}
	};

	if (!useCounterWiseForFrontFace)
	{
		for (auto& polygon : polygons)
		{
			std::swap(polygon[1], polygon[2]);
		}
	}

	cd::Mesh mesh(static_cast<uint32_t>(positions.size()), static_cast<uint32_t>(polygons.size()));

	for (uint32_t i = 0; i < positions.size(); ++i)
	{
		mesh.SetVertexPosition(i, positions[i]);
	}

	for (uint32_t i = 0; i < polygons.size(); ++i)
	{
		mesh.SetPolygon(i, polygons[i]);
	}

	cd::VertexFormat meshVertexFormat;
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);

	if (vertexFormat.Contains(VertexAttributeType::Normal))
	{
		mesh.ComputeVertexNormals();
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::UV))
	{
		mesh.SetVertexUVSetCount(1);
		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			const auto& position = mesh.GetVertexPosition(vertexIndex);
			mesh.SetVertexUV(0U, vertexIndex, cd::UV(position.x(), position.z()));
		}

		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}


	if (vertexFormat.Contains(VertexAttributeType::Tangent) || vertexFormat.Contains(VertexAttributeType::Bitangent))
	{
		mesh.ComputeVertexTangents();
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	// Use VertexColor0 to present braycentric coordinates.
	if (vertexFormat.Contains(VertexAttributeType::Color))
	{
		mesh.SetVertexColorSetCount(1U);
		mesh.SetVertexColor(0U, 0U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 1U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 2U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 3U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 4U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 5U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 6U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 7U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Vec4f::ValueType>(), cd::Vec4f::Size);
	}

	mesh.SetVertexFormat(MoveTemp(meshVertexFormat));
	mesh.SetAABB(AABB(min, max));

	return mesh;
}

std::optional<Mesh> MeshGenerator::Generate(const Sphere& sphere, uint32_t numStacks, uint32_t numSlices, const VertexFormat& vertexFormat)
{
	assert(vertexFormat.Contains(VertexAttributeType::Position));

	uint32_t vertexCount = (numStacks + 1) * (numSlices + 1);
	uint32_t polygonCount = numStacks * numSlices * 2;

	cd::Mesh mesh(vertexCount, polygonCount);
	cd::VertexFormat meshVertexFormat;
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	
	bool generateUV = vertexFormat.Contains(VertexAttributeType::UV);
	if (generateUV)
	{
		mesh.SetVertexUVSetCount(1);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}

	// Generate vertices
	uint32_t vertexIndex = 0U;
	float radius = sphere.Radius();
	for (uint32_t stackIndex = 0U; stackIndex <= numStacks; ++stackIndex)
	{
		float v = static_cast<float>(stackIndex) / static_cast<float>(numStacks);
		float theta = v * Math::PI;
		float sinTheta = std::sin(theta);
		float cosTheta = std::cos(theta);

		for (uint32_t sliceIndex = 0U; sliceIndex <= numSlices; ++sliceIndex)
		{
			float u = static_cast<float>(sliceIndex) / static_cast<float>(numSlices);
			float phi = u * Math::TWO_PI;
			float sinPhi = std::sin(phi);
			float cosPhi = std::cos(phi);
			mesh.SetVertexPosition(vertexIndex, cd::Point(radius * sinTheta * cosPhi, radius * sinTheta * sinPhi, radius * cosTheta));
		
			if (generateUV)
			{
				mesh.SetVertexUV(0, vertexIndex, cd::UV(u, v));
			}

			++vertexIndex;
		}
	}

	// Generate indices
	uint32_t polygonIndex = 0U;
	for (uint32_t stackIndex = 0U; stackIndex < numStacks; ++stackIndex)
	{
		uint32_t stackStart = stackIndex * (numSlices + 1);
		uint32_t stackEnd = stackStart + numSlices + 1;

		for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
		{
			mesh.SetPolygon(polygonIndex++, cd::Polygon(stackStart + sliceIndex, stackEnd + sliceIndex, stackEnd + sliceIndex + 1));
			mesh.SetPolygon(polygonIndex++, cd::Polygon(stackStart + sliceIndex, stackEnd + sliceIndex + 1, stackStart + sliceIndex + 1));
		}
	}

	// Add other vertex attributes.
	if (vertexFormat.Contains(VertexAttributeType::Normal))
	{
		mesh.ComputeVertexNormals();
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::Tangent) || vertexFormat.Contains(VertexAttributeType::Bitangent))
	{
		mesh.ComputeVertexTangents();
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	if (vertexFormat.Contains(VertexAttributeType::Color))
	{
		mesh.SetVertexColorSetCount(1U);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Vec4f::ValueType>(), cd::Vec4f::Size);
	}

	mesh.SetVertexFormat(MoveTemp(meshVertexFormat));
	cd::Point min = sphere.Center() - cd::Vec3f(radius);
	cd::Point max = sphere.Center() + cd::Vec3f(radius);
	mesh.SetAABB(cd::AABB(min, max));

	return mesh;
}

}