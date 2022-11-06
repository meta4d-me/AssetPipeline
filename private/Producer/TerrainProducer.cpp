#include "TerrainProducer.h"

#include <cassert>

#include "Math/VectorDerived.hpp"
#include "Noise/Noise.h"
#include "Scene/Mesh.h"
#include "Scene/ObjectIDTypes.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/Utils.h"

namespace cdtools
{

TerrainProducer::TerrainProducer(uint32_t width, uint32_t height)
	: m_width(width)
	, m_height(height)
{
	// Must be at least 2 vertices for each dimension
	assert(m_width > 1);
	assert(m_height > 1);
}

void TerrainProducer::Execute(SceneDatabase* pSceneDatabase) 
{
	uint32_t num_vertices = m_width * m_height;
	//2 indices per square and 2 triangles per square
	uint32_t num_polygons = (m_width - 1) * (m_height - 1) * 2;
	pSceneDatabase->SetMeshCount(1);
	Mesh terrain(MeshID(pSceneDatabase->GetNextMeshID()), "GeneratedTerrain", num_vertices, num_polygons);
	
	// We will start at the bottom left corner from bottom to top, left to right
	// ...
	// 12 13 14 15
	//  8  9 10 11
	//  4  5  6  7 
	//  0  1  2  3 
	uint32_t current_vertex_id = 0;
	for (uint32_t z = 0; z < m_height; ++z)
	{
		for (uint32_t x = 0; x < m_width; ++x)
		{
			// Increment ID
			terrain.SetVertexPosition(current_vertex_id, Point(static_cast<float>(x), 0.0f, static_cast<float>(z)));
			terrain.SetVertexNormal(current_vertex_id, Direction(0.0f, 1.0f, 0.0f));
			++current_vertex_id;
		}
	}

	// Sets the indices
	uint32_t polygonIndex = 0;
	uint32_t currentVertexIndex = m_width;
	for (uint32_t z = 1; z < m_height; ++z) {
		for (uint32_t x = 0; x < (m_width - 1); ++x)
		{
			VertexID topLeftVertex(currentVertexIndex);
			VertexID topRigthVertex(currentVertexIndex + 1);
			VertexID bottomLeftVertex(currentVertexIndex - m_width);
			VertexID bottomRightVertex(currentVertexIndex - m_width + 1);
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

	pSceneDatabase->AddMesh(terrain);
}

}	// namespace cdtools