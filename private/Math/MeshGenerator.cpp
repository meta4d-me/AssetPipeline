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

	//          0 --- 1 (max)
	//        / |   / |
	//       4 --- 5  |
	//       |  |  |  |
	//       |  2 -|- 3
	//       | /   | /
	// (min) 6 --- 7

	std::array<cd::Point, 24> positions = {
		cd::Point(min.x(), max.y(), max.z()),//0
		cd::Point(max.x(), max.y(), max.z()),//1
		cd::Point(min.x(), min.y(), max.z()),//2
		cd::Point(max.x(), min.y(), max.z()),//3

		cd::Point(min.x(), max.y(), min.z()),//4
		cd::Point(max.x(), max.y(), min.z()),//5
		cd::Point(min.x(), min.y(), min.z()),//6
		cd::Point(max.x(), min.y(), min.z()),//7

		cd::Point(max.x(), max.y(), max.z()),//1
		cd::Point(max.x(), min.y(), max.z()),//3
		cd::Point(max.x(), max.y(), min.z()),//5
		cd::Point(max.x(), min.y(), min.z()),//7

		cd::Point(min.x(), max.y(), max.z()),//0
		cd::Point(min.x(), min.y(), max.z()),//2
		cd::Point(min.x(), max.y(), min.z()),//4
		cd::Point(min.x(), min.y(), min.z()),//6

		cd::Point(min.x(), max.y(), max.z()),//0
		cd::Point(max.x(), max.y(), max.z()),//1
		cd::Point(min.x(), max.y(), min.z()),//4
		cd::Point(max.x(), max.y(), min.z()),//5

		cd::Point(min.x(), min.y(), max.z()),//2
		cd::Point(max.x(), min.y(), max.z()),//3
		cd::Point(min.x(), min.y(), min.z()),//6
		cd::Point(max.x(), min.y(), min.z())//7
	};

	std::array<cd::Polygon, 12> polygons = {
		cd::Polygon{3U, 1U, 0U},
		cd::Polygon{0U, 2U, 3U},

		cd::Polygon{11U, 10U, 8U},//7 5 1
		cd::Polygon{8U, 9U, 11U},//1 3 7

		cd::Polygon{5U, 7U, 6U},
		cd::Polygon{6U, 4U, 5U},

		cd::Polygon{17U, 19U, 18U},//1 5 4
		cd::Polygon{18U, 16U, 17U},//4 0 1

		cd::Polygon{12U, 14U, 15U},//0 4 6
		cd::Polygon{15U, 13U, 12U},//6 2 0

		cd::Polygon{20U, 22U, 23U},//2 6 7
		cd::Polygon{23U, 21U, 20U},//7 3 2 
	};

	if (!useCounterWiseForFrontFace)
	{
		for (auto& polygon : polygons)
		{
			std::swap(polygon[1], polygon[2]);
		}
	}

	cd::Mesh mesh(static_cast<uint32_t>(positions.size()));

	for (uint32_t i = 0U; i < positions.size(); ++i)
	{
		mesh.SetVertexPosition(i, positions[i]);
	}

	cd::PolygonGroup polygonGroup;
	for (uint32_t i = 0U; i < polygons.size(); ++i)
	{
		polygonGroup.push_back(polygons[i]);
	}
	mesh.AddPolygonGroup(cd::MoveTemp(polygonGroup));

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
		mesh.SetVertexColor(0U, 8U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 9U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 10U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 11U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 12U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 13U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 14U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 15U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 16U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 17U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 18U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 19U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 20U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
		mesh.SetVertexColor(0U, 21U, cd::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 22U, cd::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
		mesh.SetVertexColor(0U, 23U, cd::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
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

	cd::Mesh mesh(vertexCount);
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
	uint32_t stackIndex = 0U;
	uint32_t stackStart = stackIndex * (numSlices + 1);
	uint32_t stackEnd = stackStart + numSlices + 1;
	cd::PolygonGroup polygonGroup;
	for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
	{
		polygonGroup.push_back({ stackStart + sliceIndex, stackEnd + sliceIndex, stackEnd + sliceIndex + 1 });
	}

	for (stackIndex = 1U; stackIndex < numStacks - 1; ++stackIndex)
	{
		stackStart = stackIndex * (numSlices + 1);
		stackEnd = stackStart + numSlices + 1;

		for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
		{
			polygonGroup.push_back({ stackStart + sliceIndex, stackEnd + sliceIndex, stackEnd + sliceIndex + 1 });
			polygonGroup.push_back({ stackStart + sliceIndex, stackEnd + sliceIndex + 1, stackStart + sliceIndex + 1 });
		}
	}

	stackStart = stackIndex * (numSlices + 1);
	stackEnd = stackStart + numSlices + 1;
	for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
	{
		polygonGroup.push_back({ stackStart + sliceIndex, stackEnd + sliceIndex, stackStart + sliceIndex + 1 });
	}
	mesh.AddPolygonGroup(cd::MoveTemp(polygonGroup));

	// Add other vertex attributes.
	if (vertexFormat.Contains(VertexAttributeType::Normal))
	{
		mesh.ComputeVertexNormals();

		{//correct normal
			uint32_t stackIndex = 0U;
			Direction correctedNormal = Direction(0, 0, 0);
			for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
			{
				correctedNormal += mesh.GetVertexNormal(sliceIndex);
			}
			correctedNormal.Normalize();
			for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
			{
				mesh.SetVertexNormal(sliceIndex, correctedNormal);
			}

			for (stackIndex = 1U; stackIndex < numStacks; ++stackIndex)
			{
				uint32_t stackStart = stackIndex * (numSlices + 1);
				uint32_t stackEnd = stackStart + numSlices;
				correctedNormal = mesh.GetVertexNormal(stackStart) + mesh.GetVertexNormal(stackEnd);
				correctedNormal.Normalize();
				mesh.SetVertexNormal(stackStart, correctedNormal);
				mesh.SetVertexNormal(stackEnd, correctedNormal);
			}

			for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
			{
				correctedNormal += mesh.GetVertexNormal(stackIndex * (numSlices + 1) + sliceIndex);
			}
			correctedNormal.Normalize();
			for (uint32_t sliceIndex = 0U; sliceIndex < numSlices; ++sliceIndex)
			{
				mesh.SetVertexNormal(stackIndex * (numSlices + 1) + sliceIndex, correctedNormal);
			}
		}

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
