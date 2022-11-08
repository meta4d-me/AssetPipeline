#include "TerrainProducer.h"

#include <cassert>
#include <climits>
#include <cmath>
#include <random>

#include "Math/VectorDerived.hpp"
#include "Noise/Noise.h"
#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/Utils.h"

namespace cdtools
{

TerrainProducer::TerrainProducer(uint32_t x_vertices, uint32_t z_vertices, uint32_t width, uint32_t height, uint32_t max_elevation)
	: m_numVerticesInX(x_vertices)
	, m_numVerticesInZ(z_vertices)
	, m_width(width)
	, m_height(height)
	, m_maxElevation(max_elevation)
{
	// Must be at least 2 vertices for each dimension
	assert(m_numVerticesInZ > 1);
	assert(m_numVerticesInX > 1);
}

void TerrainProducer::Execute(SceneDatabase* pSceneDatabase) 
{
	uint32_t num_vertices = m_numVerticesInZ * m_numVerticesInX;
	//2 vertices makes an edge and 2 triangles per square
	uint32_t num_polygons = (m_numVerticesInZ - 1) * (m_numVerticesInX - 1) * 2;
	pSceneDatabase->SetMeshCount(1);
	Mesh terrain(MeshID(pSceneDatabase->GetNextMeshID()), "GeneratedTerrain", num_vertices, num_polygons);

	// Setup the random seeds for noise functions
	std::default_random_engine generator;
	std::uniform_int_distribution<long> distribution(LONG_MIN, LONG_MAX);
	
	// First generate the set of vertices
	// We will start at the bottom left corner from bottom to top, left to right
	// ...
	// 12 13 14 15
	//  8  9 10 11
	//  4  5  6  7 
	//  0  1  2  3 
	// Seeds
	const int64_t s0 = distribution(generator);
	const int64_t s1 = distribution(generator);
	const int64_t s2 = distribution(generator);
	const int64_t s3 = distribution(generator);
	const int64_t s4 = distribution(generator);
	const int64_t s5 = distribution(generator);
	// Distributions between seeds
	const float e0 = 0.0;
	const float e1 = 0.8;
	const float e2 = 0.8;
	const float e3 = 0.5;
	const float e4 = 0.4;
	const float e5 = 0.6;
	const float esum = e0 + e1 + e2 + e3 + e4 + e5;
	int32_t current_vertex_id = 0;
	for (uint32_t z = 0; z < m_numVerticesInZ; ++z)
	{
		for (uint32_t x = 0; x < m_numVerticesInX; ++x)
		{
			const double nx = static_cast<float>(x) / m_numVerticesInX - 0.5;
			const double nz = static_cast<float>(z) / m_numVerticesInZ - 0.5;
			float elevation = e0 * Noise::SimplexNoise2D(s0, 1.0 * nx, 1.0 * nz)
							+ e1 * Noise::SimplexNoise2D(s1, 2.0 * nx, 2.0 * nz)
							+ e2 * Noise::SimplexNoise2D(s2, 4.0 * nx, 4.0 * nz)
							+ e3 * Noise::SimplexNoise2D(s3, 8.0 * nx, 8.0 * nz)
							+ e4 * Noise::SimplexNoise2D(s4, 16.0 * nx, 16.0 * nz)
							+ e5 * Noise::SimplexNoise2D(s5, 32.0 * nx, 32.0 * nz);
			elevation /= esum;	//normalize
			elevation = pow(elevation, 6.5);
			elevation *= m_maxElevation;
			terrain.SetVertexPosition(static_cast<uint32_t>(current_vertex_id), Point(static_cast<float>(x) * m_width, elevation, static_cast<float>(z) * m_height));
			// Increment ID
			++current_vertex_id;
		}
	}

	/*				\|   \|   \|   \|              z
	 *			...--+----UL---U----+---+...       ^
	 *               |\   |\   |\   |\  |          |
	 *               | \  | \  | \  | \ |          |
	 *               |  \ |  \ |  \ |  \|          |
	 *          ...--+----L----P----R---+...       +------> x
	 *               |\   |\   |\   |\  |
	 *               | \  | \  | \  | \ |
	 *               |  \ |  \ |  \ |  \|
	 *			...--+----+----B----BR--+...
	 */
	// For each vertices, calculate the normal. Here we will use the 6 neighboring vertices
	// We have the following pairs
	// P-UL X P-L; P-L X P-B; P-B X P-BR; P-BR X PR; P-R X P-U; P-U X P-UL
	for (uint32_t z = 0; z < m_numVerticesInZ; ++z)
	{
		for (uint32_t x = 0; x < m_numVerticesInX; ++x)
		{
			// Calculate all the 6 vertices IDs
			current_vertex_id = z * m_numVerticesInX + x;
			int32_t leftVertex = current_vertex_id - 1;
			int32_t rightVertex = current_vertex_id + 1;
			int32_t topVertex = current_vertex_id + m_numVerticesInX;
			int32_t bottomVertex = current_vertex_id - m_numVerticesInX;
			int32_t topLeftVertex = topVertex - 1;
			int32_t bottomRightVertex = bottomVertex + 1;
			// Calculate all the 6 directions from current vertex
			Direction currentToLeftDir;
			Direction currentToRightDir;
			Direction currentToTopDir;
			Direction currentToBottomDir;
			Direction currentToTopLeftDir;
			Direction currentToBottomRightDir;
			// Check the boundaries
			const bool hasLeft = x > 0;
			const bool hasRight = x < static_cast<int32_t>(m_numVerticesInX - 1);
			const bool hasTop = z < static_cast<int32_t>(m_numVerticesInZ - 1);
			const bool hasBottom = z > 0;
			// Calculate the directions
			if (hasLeft) {
				currentToLeftDir = terrain.GetVertexPosition(static_cast<uint32_t>(leftVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			if (hasRight) {
				currentToRightDir = terrain.GetVertexPosition(static_cast<uint32_t>(rightVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			if (hasTop) {
				currentToTopDir = terrain.GetVertexPosition(static_cast<uint32_t>(topVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			if (hasBottom) {
				currentToBottomDir = terrain.GetVertexPosition(static_cast<uint32_t>(bottomVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			if (hasTop && hasLeft) {
				currentToTopLeftDir = terrain.GetVertexPosition(static_cast<uint32_t>(topLeftVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			if (hasBottom && hasRight) {
				currentToBottomRightDir = terrain.GetVertexPosition(static_cast<uint32_t>(bottomRightVertex)) - terrain.GetVertexPosition(static_cast<uint32_t>(current_vertex_id));
			}
			// Calculate the normal based on all these
			Direction normal;
			float topToTopLeftArea = 0;
			float topLeftToLeftArea = 0;
			float leftToBottomArea = 0;
			float bottomToBottomRightArea = 0;
			float bottomRightToRightArea = 0;
			float rightToTopArea = 0;
			Direction topToTopLeftNormal;
			Direction topLeftToLeftNormal;
			Direction leftToBottomNormal;
			Direction bottomToBottomRightNormal;
			Direction bottomRightToRightNormal;
			Direction rightToTopNormal;
			if (hasTop && hasLeft) {
				topToTopLeftNormal = currentToTopDir.Cross(currentToTopLeftDir);
				topToTopLeftArea = topToTopLeftNormal.LengthSquare();
				topLeftToLeftNormal = currentToTopLeftDir.Cross(currentToLeftDir);
				topLeftToLeftArea = topLeftToLeftNormal.LengthSquare();
			}
			if (hasLeft && hasBottom) {
				leftToBottomNormal = currentToBottomDir.Cross(currentToLeftDir);
				leftToBottomArea = leftToBottomNormal.LengthSquare();
			}
			if (hasBottom && hasRight) {
				bottomToBottomRightNormal = currentToBottomDir.Cross(currentToBottomRightDir);
				bottomToBottomRightArea = bottomToBottomRightNormal.LengthSquare();
				bottomRightToRightNormal = currentToBottomRightDir.Cross(currentToRightDir);
				bottomRightToRightArea = bottomRightToRightNormal.LengthSquare();
			}
			if (hasRight && hasTop) {
				rightToTopNormal = currentToTopDir.Cross(currentToRightDir);
				rightToTopArea = rightToTopNormal.LengthSquare();
			}
			float total_area = topToTopLeftArea + topLeftToLeftArea + leftToBottomArea + bottomToBottomRightArea + bottomRightToRightArea + rightToTopArea;
			if (total_area <= 0) {
				assert(false);
			}
			// Calculate the normal
			normal.Add(topToTopLeftNormal.Multiply(topLeftToLeftArea / total_area));
			normal.Add(topLeftToLeftNormal.Multiply(topLeftToLeftArea / total_area));
			normal.Add(leftToBottomNormal.Multiply(leftToBottomArea / total_area));
			normal.Add(bottomToBottomRightNormal.Multiply(bottomToBottomRightArea / total_area));
			normal.Add(bottomRightToRightNormal.Multiply(bottomRightToRightArea / total_area));
			normal.Add(rightToTopNormal.Multiply(rightToTopArea / total_area));
			normal.Normalize();
			terrain.SetVertexNormal(static_cast<uint32_t>(current_vertex_id), normal);
		}
	}

	// Sets the indices
	uint32_t polygonIndex = 0;
	uint32_t currentVertexIndex = m_numVerticesInZ;
	for (uint32_t z = 1; z < m_numVerticesInX; ++z) {
		for (uint32_t x = 0; x < (m_numVerticesInZ - 1); ++x)
		{
			VertexID topLeftVertex(currentVertexIndex);
			VertexID topRigthVertex(currentVertexIndex + 1);
			VertexID bottomLeftVertex(currentVertexIndex - m_numVerticesInZ);
			VertexID bottomRightVertex(currentVertexIndex - m_numVerticesInZ + 1);
			terrain.SetPolygon(polygonIndex, topLeftVertex, bottomRightVertex, bottomLeftVertex);
			++polygonIndex;
			terrain.SetPolygon(polygonIndex, topLeftVertex, topRigthVertex, bottomRightVertex);
			++polygonIndex;
			++currentVertexIndex;
		}
		++currentVertexIndex;	// Skip last vertex
	}
	assert(polygonIndex == num_polygons);

	terrain.SetVertexColorSetCount(0);
	terrain.SetVertexUVSetCount(0);

	pSceneDatabase->AddMesh(std::move(terrain));
}

}	// namespace cdtools