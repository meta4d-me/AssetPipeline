#pragma once

#include <stdint.h>
#include <vector>

#include "Math/AABB.hpp"
#include "Math/VectorDerived.hpp"
#include "Producer/IProducer.h"
#include "Scene/ObjectIDGenerator.h"

namespace cd
{

class Mesh;

}

namespace cdtools
{

struct TerrainGenParams
{
	uint16_t numSectorsInX;									// number of sectors in x-axis
	uint16_t numSectorsInZ;									// number of sectors in z-axis
	uint16_t numQuadsInSectorInX;							// number of quads in x-axis for each sector
	uint16_t numQuadsInSectorInZ;							// number of quads in z-axis for each sector
	uint16_t quadLengthInX;									// the length of each quad in x-axis
	uint16_t quadLengthInZ;									// the length of each quad in z-axis
	int32_t minHeight;										// the minimum height (can be negative) of this entire terrain
	int32_t maxHeight;										// the maximum height of this entire terrain
	std::vector<std::pair<float, int64_t>> weightAndSeeds;	// Set of weights and seeds at powers of 2 frequencies
};

struct TerrainQuad 
{
	uint32_t leftTriPolygonId;
	uint32_t rightTriPolygonId;
	uint32_t bottomLeftVertexId;
	uint32_t topLeftVertexId;
	uint32_t topRightVertexId;
	uint32_t bottomRightVertexId;
	cd::Direction bottomLeftNormal;
	cd::Direction topLeftNormal;
	cd::Direction topRightNormal;
	cd::Direction bottomRightNormal;
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

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	cd::Mesh CreateTerrainMesh();
	TerrainQuad CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const;
	float GetHeightAt(uint32_t x, uint32_t z, const std::vector<std::pair<float, int64_t>>& freq_params, float power_exp) const;

	uint16_t m_numSectors;		// number of sectors to generate
	uint32_t m_numQuadsInX;		// quads in x-axis
	uint32_t m_numQuadsInZ;		// quads in z-axis
	uint32_t m_quadWidth;		// width of a quad
	uint32_t m_quadHeight;		// height of a quad
	uint32_t m_maxElevation;	// highest possible elevation; we start at 0.

	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
};

}	// namespace cdtools
