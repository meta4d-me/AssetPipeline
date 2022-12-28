#pragma once

#include "Scene/ObjectIDGenerator.h"
#include "Producers/TerrainProducer/TerrainParameter.h"

namespace cd
{

class Mesh;
class SceneDatabase;

}

namespace cdtools
{

class TerrainProducerImpl
{
public:
	TerrainProducerImpl() = delete;
	explicit TerrainProducerImpl(const TerrainGenParams& genParams);
	TerrainProducerImpl(const TerrainProducerImpl& rhs) = delete;
	TerrainProducerImpl& operator=(const TerrainProducerImpl& rhs) = delete;
	TerrainProducerImpl(TerrainProducerImpl&& rhs) = delete;
	TerrainProducerImpl& operator=(TerrainProducerImpl&& rhs) = delete;
	~TerrainProducerImpl() = default;

	void Execute(cd::SceneDatabase* pSceneDatabase);
	cd::Mesh CreateTerrainMesh(uint32_t sector_x, uint32_t sector_z);
	TerrainQuad CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const;

private:
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
	std::vector<HeightOctave> m_octaves;

	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
};

}	// namespace cdtools
