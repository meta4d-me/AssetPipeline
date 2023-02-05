#pragma once

#include "Framework/IProducer.h"

#include <stdint.h>

namespace cd
{

class Mesh;

}

namespace cdtools
{

class TerrainProducerImpl;
struct TerrainMetadata;
struct TerrainSectorMetadata;

class TOOL_API TerrainProducer : public IProducer
{
public:
	TerrainProducer() = delete;
	explicit TerrainProducer(const cdtools::TerrainMetadata& terrainMetadata, const cdtools::TerrainSectorMetadata& sectorMetadata);
	TerrainProducer(const TerrainProducer& rhs) = delete;
	TerrainProducer& operator=(const TerrainProducer& rhs) = delete;
	TerrainProducer(TerrainProducer&& rhs) = delete;
	TerrainProducer& operator=(TerrainProducer&& rhs) = delete;
	virtual ~TerrainProducer();

	void SetSceneDatabaseIDs(const cd::SceneDatabase* pSceneDatabase);
	void SetTerrainMetadata(const cdtools::TerrainMetadata& metadata);
	void SetSectorMetadata(const cdtools::TerrainSectorMetadata& metadata);
	void Initialize();

	uint32_t GetTerrainLengthInX() const;
	uint32_t GetTerrainLengthInZ() const;
	uint32_t GetSectorCount() const;
	uint32_t GetSectorLengthInX() const;
	uint32_t GetSectorLengthInZ() const;
	uint32_t GetQuadsPerSector() const;
	uint32_t GetVertsPerSector() const;

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	TerrainProducerImpl* m_pTerrainProducerImpl;
};

}	// namespace cdtools
