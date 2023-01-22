#pragma once

#include "Framework/IProducer.h"

#include <stdint.h>

namespace cd
{

class Mesh;
struct TerrainMetadata;
struct TerrainSectorMetadata;

}

namespace cdtools
{

class TerrainProducerImpl;

class TOOL_API TerrainProducer : public IProducer
{
public:
	TerrainProducer() = delete;
	explicit TerrainProducer(const cd::TerrainMetadata& terrainMetadata, const cd::TerrainSectorMetadata& sectorMetadata);
	TerrainProducer(const TerrainProducer& rhs) = delete;
	TerrainProducer& operator=(const TerrainProducer& rhs) = delete;
	TerrainProducer(TerrainProducer&& rhs) = delete;
	TerrainProducer& operator=(TerrainProducer&& rhs) = delete;
	virtual ~TerrainProducer();

	void SetSceneDatabaseIDs(const cd::SceneDatabase* pSceneDatabase);
	void SetTerrainMetadata(const cd::TerrainMetadata& metadata);
	void SetSectorMetadata(const cd::TerrainSectorMetadata& metadata);
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
