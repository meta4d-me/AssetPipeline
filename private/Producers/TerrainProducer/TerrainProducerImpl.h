#pragma once

#include "AlphaMap.h"
#include "Scene/ObjectIDGenerator.h"
#include "TerrainProducer/AlphaMapTypes.h"
#include "TerrainProducer/TerrainTypes.h"

#include <memory>

namespace cd
{

class Material;
class Mesh;
class SceneDatabase;
class Texture;

}

namespace cdtools
{

class TerrainProducerImpl
{
public:
	TerrainProducerImpl() = delete;
	explicit TerrainProducerImpl(const cdtools::TerrainMetadata& terrainMetadata, const cdtools::TerrainSectorMetadata& sectorMetadata);
	TerrainProducerImpl(const TerrainProducerImpl& rhs) = delete;
	TerrainProducerImpl& operator=(const TerrainProducerImpl& rhs) = delete;
	TerrainProducerImpl(TerrainProducerImpl&& rhs) = delete;
	TerrainProducerImpl& operator=(TerrainProducerImpl&& rhs) = delete;
	~TerrainProducerImpl() = default;

	void SetSceneDatabaseIDs(const cd::SceneDatabase* pSceneDatabase);
	void SetTerrainMetadata(const TerrainMetadata& metadata);
	void SetSectorMetadata(const TerrainSectorMetadata& metadata);
	void SetAlphaMapTextureName(AlphaMapChannel channel, const std::string_view textureName);
	void RemoveAlphaMapGeneration();
	void Initialize();

	uint32_t GetTerrainLengthInX() const 
	{
		return m_terrainLenInX;
	}

	uint32_t GetTerrainLengthInZ() const 
	{
		return m_terrainLenInZ;
	}

	uint32_t GetSectorCount() const 
	{
		return m_sectorCount;
	}

	uint32_t GetSectorLengthInX() const 
	{
		return m_sectorLenInX;
	}

	uint32_t GetSectorLengthInZ() const 
	{
		return m_sectorLenInZ;
	}

	uint32_t GetQuadsPerSector() const 
	{
		return m_quadsPerSector;
	}

	uint32_t GetVertsPerSector() const 
	{
		return m_verticesPerSector;
	}

	const std::string_view GetAlphaMapTextureName(AlphaMapChannel channel) const
	{
		assert(channel != AlphaMapChannel::Count);
		return m_alphaMapTextureNames[static_cast<uint8_t>(channel)];
	}

	void GenerateAlphaMapWithElevation(
		const AlphaMapBlendRegion<int32_t>& redGreenRegion,
		const AlphaMapBlendRegion<int32_t>& greenBlueRegion,
		const AlphaMapBlendRegion<int32_t>& blueAlphaRegion,
		const AlphaMapBlendFunction& blendFunction);

	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	cdtools::TerrainMetadata m_terrainMetadata;
	cdtools::TerrainSectorMetadata m_sectorMetadata;
	uint32_t m_terrainLenInX;
	uint32_t m_terrainLenInZ;
	uint32_t m_sectorCount;
	uint32_t m_sectorLenInX;
	uint32_t m_sectorLenInZ;
	uint32_t m_quadsPerSector;
	uint32_t m_verticesPerSector;
	uint32_t m_trianglesPerSector;
	std::array<std::string, 4> m_alphaMapTextureNames;
	std::unique_ptr<ElevationAlphaMapDef> m_pElevationAlphaMapDef = nullptr;
	std::unique_ptr<NoiseAlphaMapDef> m_pNoiseAlphaMapDef = nullptr;

	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;

	void GenerateElevationMap(std::vector<int32_t>& outElevationMap, uint32_t sector_x, uint32_t sector_z) const;
	void GenerateElevationBasedAlphaMap(std::vector<uint32_t>& outAlphaMap, const std::vector<int32_t>& elevationMap) const;
	void GenerateAllSectors(cd::SceneDatabase* pSceneDatabase);
	cd::Mesh GenerateSectorAt(uint32_t sector_x, uint32_t sector_z, const std::vector<int32_t>& elevationMap);
	cd::MaterialID GenerateMaterialAndTextures(cd::SceneDatabase* pSceneDatabase, uint32_t sector_x, uint32_t sector_z, std::vector<int32_t>& elevationMap);
};

}	// namespace cdtools
