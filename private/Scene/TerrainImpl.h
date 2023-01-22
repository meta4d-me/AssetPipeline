#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/ObjectID.h"
#include "Scene/TerrainTypes.h"
#include "Scene/Texture.h"

#include <vector>

namespace cd
{

class TerrainImpl final
{
public:
	// We expect to use triangulated mesh data in game engine.
	using Polygon = TVector<VertexID, 3>;

public:
	TerrainImpl() = delete;

	template<bool SwapBytesOrder>
	explicit TerrainImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit TerrainImpl(const TerrainMetadata& terrainMetadata, const TerrainSectorMetadata& sectorMetadata);

	TerrainImpl(const TerrainImpl&) = default;
	TerrainImpl& operator=(const TerrainImpl&) = default;
	TerrainImpl(TerrainImpl&&) = default;
	TerrainImpl& operator=(TerrainImpl&&) = default;
	~TerrainImpl() = default;

	void SetMaterialID(uint32_t materialIndex) { m_terrainMaterialID = materialIndex; }
	const MaterialID& GetMaterialID() const { return m_terrainMaterialID; }

	template<bool SwapBytesOrder>
	TerrainImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t materialID;
		bool dirty;
		size_t elevationLookupSize;
		size_t sectorSize;
		size_t textureSize;

		m_terrainMetadata = TerrainMetadata(inputArchive);
		m_sectorMetadata = TerrainSectorMetadata(inputArchive);
		inputArchive >> materialID >> dirty >> elevationLookupSize >> sectorSize >> textureSize;
		m_elevationLookup.resize(elevationLookupSize);
		inputArchive.ImportBuffer(m_elevationLookup.data());
		uint32_t i = 0;
		m_sectors.reserve(sectorSize);
		for (i = 0; i < sectorSize; ++i) {
			m_sectors.emplace_back(inputArchive);
		}
		m_textures.reserve(textureSize);
		for (i = 0; i < textureSize; ++i) {
			m_textures.emplace_back(inputArchive);
		}
		
		m_terrainMaterialID = MaterialID(materialID);
		m_dirty = dirty;

		return *this;
	}

	template<bool SwapBytesOrder>
	const TerrainImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		m_terrainMetadata >> outputArchive;
		m_sectorMetadata >> outputArchive;
		outputArchive << m_terrainMaterialID.Data() << m_dirty
			<< m_elevationLookup.size() << m_sectors.size() << m_textures.size();
		outputArchive.ExportBuffer(m_elevationLookup.data(), m_elevationLookup.size());
		uint32_t i = 0;
		for (i = 0; i < m_sectors.size(); ++i)
		{
			m_sectors[i] >> outputArchive;
		}
		for (i = 0; i < m_textures.size(); ++i)
		{
			m_textures[i] >> outputArchive;
		}
		return *this;
	}

	void SetTerrainMetadata(const TerrainMetadata& metadata);
	void SetSectorMetadata(const TerrainSectorMetadata& metadata);

	const TerrainMetadata& GetTerrainMetadata() const;
	const TerrainSectorMetadata& GetSectorMetadata() const;
	const std::vector<int32_t>& GetElevationMap() const;
	const std::vector<Mesh>& GetSectors() const;

private:
	TerrainMetadata m_terrainMetadata;
	TerrainSectorMetadata m_sectorMetadata;
	std::vector<int32_t> m_elevationLookup;	// Stores elevation data in the range of (INT_MIN, INT_MAX) which is normalized to min/max elevation in metadata
	MaterialID m_terrainMaterialID;
	std::vector<Mesh> m_sectors;
	std::vector<Texture> m_textures;
	bool m_dirty;

	void Regenerate();
	void GenerateElevationLookup();
	void GenerateAllSectors();
	Mesh GenerateSectorAt(uint32_t sector_x, uint32_t sector_z);
};

}