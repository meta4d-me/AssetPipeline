#pragma once

#include "HeightFunctions.h"
#include "Math/AABB.hpp"
#include "Math/VectorDerived.hpp"
#include "Producer/IProducer.h"
#include "Scene/ObjectIDGenerator.h"

#include <stdint.h>
#include <vector>

namespace cd
{

class Mesh;

}

namespace cdtools
{

struct TerrainGenParams
{
	uint16_t numSectorsInX;							// number of sectors in x-axis
	uint16_t numSectorsInZ;							// number of sectors in z-axis
	uint16_t numQuadsInSectorInX;					// number of quads in x-axis for each sector
	uint16_t numQuadsInSectorInZ;					// number of quads in z-axis for each sector
	uint16_t quadLengthInX;							// the length of each quad in x-axis
	uint16_t quadLengthInZ;							// the length of each quad in z-axis
	int32_t minElevation;							// the minimum elevation (can be negative) of this entire terrain
	int32_t maxElevation;							// the maximum elevation of this entire terrain
	std::vector<HeightFunctions::Octave> octaves;	// Set of weights and seeds at powers of 2 frequencies
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
	explicit TerrainProducer(const TerrainGenParams& genParams);
	TerrainProducer(const TerrainProducer& rhs) = default;
	TerrainProducer& operator=(const TerrainProducer& rhs) = default;
	TerrainProducer(TerrainProducer&& rhs) = default;
	TerrainProducer& operator=(TerrainProducer&& rhs) = default;
	virtual ~TerrainProducer() = default;

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	cd::Mesh CreateTerrainMesh(uint32_t sector_x, uint32_t sector_z);
	TerrainQuad CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const;

	uint16_t m_numSectorsInX;
	uint16_t m_numSectorsInZ;
	uint16_t m_numQuadsInSectorInX;
	uint16_t m_numQuadsInSectorInZ;
	uint16_t m_quadLengthInX;
	uint16_t m_quadLengthInZ;
	uint32_t m_minElevation;
	uint32_t m_maxElevation;
	uint32_t m_sectorLengthInX;
	uint32_t m_sectorLengthInZ;
	uint32_t m_terrainLengthInX;
	uint32_t m_terrainLengthInZ;
	std::vector<HeightFunctions::Octave> m_octaves;

	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
};

}	// namespace cdtools
