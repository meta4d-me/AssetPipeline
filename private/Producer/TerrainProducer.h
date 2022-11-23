#pragma once

#include <stdint.h>
#include <vector>

#include "Math/AABB.hpp"
#include "Math/VectorDerived.hpp"
#include "Producer/IProducer.h"
namespace cdtools
{

class Mesh;

struct TerrainQuad {
	uint32_t leftTriPolygonId;
	uint32_t rightTriPolygonId;
	uint32_t bottomLeftVertexId;
	uint32_t topLeftVertexId;
	uint32_t topRightVertexId;
	uint32_t bottomRightVertexId;
	Direction bottomLeftNormal;
	Direction topLeftNormal;
	Direction topRightNormal;
	Direction bottomRightNormal;
};

class TerrainProducer : public IProducer
{

public:
	explicit TerrainProducer(uint32_t x_vertices, uint32_t z_vertices, uint32_t width, uint32_t height, uint32_t max_elevation);
	TerrainProducer(const TerrainProducer& rhs) = default;
	TerrainProducer& operator=(const TerrainProducer& rhs) = default;
	TerrainProducer(TerrainProducer&& rhs) = default;
	TerrainProducer& operator=(TerrainProducer&& rhs) = default;
	virtual ~TerrainProducer() = default;

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	TerrainQuad CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const;
	float GetHeightAt(uint32_t x, uint32_t z, const std::vector<std::pair<float, int64_t>>& freq_params, float power_exp) const;
	AABB CalculateAABB(const Mesh& mesh);

	uint32_t m_numQuadsInX;		// quads in x-axis
	uint32_t m_numQuadsInZ;		// quads in z-axis
	uint32_t m_quadWidth;		// width of a quad
	uint32_t m_quadHeight;		// height of a quad
	uint32_t m_maxElevation;	// highest possible elevation; we start at 0.

};

}	// namespace cdtools