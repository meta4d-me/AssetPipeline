#pragma once

#include "AlphaMapTypes.h"
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
	void SetAlphaMapTextureName(cdtools::AlphaMapChannel channel, const std::string_view textureName);
	void RemoveAlphaMapGeneration();
	void Initialize();

	uint32_t GetTerrainLengthInX() const;
	uint32_t GetTerrainLengthInZ() const;
	uint32_t GetSectorCount() const;
	uint32_t GetSectorLengthInX() const;
	uint32_t GetSectorLengthInZ() const;
	uint32_t GetQuadsPerSector() const;
	uint32_t GetVertsPerSector() const;
	const std::string_view GetAlphaMapTextureName(cdtools::AlphaMapChannel channel) const;

	void GenerateAlphaMapWithElevation(
		const AlphaMapBlendRegion<int32_t>& redGreenRegion,
		const AlphaMapBlendRegion<int32_t>& greenBlueRegion,
		const AlphaMapBlendRegion<int32_t>& blueAlphaRegion,
		const AlphaMapBlendFunction& blendFunction);

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	TerrainProducerImpl* m_pTerrainProducerImpl;
};

}	// namespace cdtools
