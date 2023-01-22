#include "TerrainImpl.h"

#include "Hashers/StringHash.hpp"
#include "Math/NoiseGenerator.h"
#include "ObjectIDGenerator.h"
#include "Scene/VertexFormat.h"

#include <cassert>

namespace
{

cd::ObjectIDGenerator<cd::MeshID> MeshIDGenerator;

float GetNoiseAt(
	const uint32_t x,
	const uint32_t z,
	const uint32_t terrainLenInX,
	const uint32_t terrainLenInZ,
	const float redistPower,
	const std::vector<cd::ElevationOctave>& octaves)
{
	const double nx = x / static_cast<double>(terrainLenInX);
	const double nz = z / static_cast<double>(terrainLenInZ);
	float height = 0.0f;
	float totalWeight = 0.0f;
	for (uint32_t i = 0; i < octaves.size(); ++i)
	{
		const cd::ElevationOctave& octave = octaves[i];
		height += octave.weight * cd::NoiseGenerator::SimplexNoise2D(octave.seed, octave.frequency * nx, octave.frequency * nz);
		totalWeight += octave.weight;
	}
	height /= totalWeight;
	height = pow(height, redistPower);
	return height;
}

}

namespace cd
{

TerrainImpl::TerrainImpl(
	const TerrainMetadata& terrainMetadata, 
	const TerrainSectorMetadata& sectorMetadata) 
	: m_terrainMetadata(terrainMetadata)
	, m_sectorMetadata(sectorMetadata)
	, m_dirty(false)
{
	// Sanity check
	assert(m_terrainMetadata.minElevation < m_terrainMetadata.maxElevation);
	assert(m_terrainMetadata.numSectorsInX >= 1);
	assert(m_terrainMetadata.numSectorsInZ >= 1);
	assert(m_sectorMetadata.numQuadsInX >= 1);
	assert(m_sectorMetadata.numQuadsInZ >= 1);
	assert(m_sectorMetadata.quadLenInX >= 1);
	assert(m_sectorMetadata.quadLenInZ >= 1);

	Regenerate();
}

void TerrainImpl::SetTerrainMetadata(const TerrainMetadata& metadata)
{
	if (m_terrainMetadata.numSectorsInX != metadata.numSectorsInX)
	{
		m_terrainMetadata.numSectorsInX = metadata.numSectorsInX;
		m_dirty = true;
	}
	if (m_terrainMetadata.numSectorsInZ != metadata.numSectorsInZ)
	{
		m_terrainMetadata.numSectorsInZ = metadata.numSectorsInZ;
		m_dirty = true;
	}
	if (m_terrainMetadata.minElevation != metadata.minElevation)
	{
		m_terrainMetadata.minElevation = metadata.minElevation;
		m_dirty = true;
	}
	if (m_terrainMetadata.maxElevation != metadata.maxElevation)
	{
		m_terrainMetadata.maxElevation = metadata.maxElevation;
		m_dirty = true;
	}
	if (m_terrainMetadata.octaves.size() != metadata.octaves.size())
	{
		m_terrainMetadata.octaves.resize(metadata.octaves.size());
		m_dirty = true;
	}
	for (uint32_t i = 0; i < metadata.octaves.size(); ++i)
	{
		m_terrainMetadata.octaves[i] = metadata.octaves[i];
		m_dirty = true;
	}
	if (m_dirty)
	{
		Regenerate();
		m_dirty = false;
	}
}

void TerrainImpl::SetSectorMetadata(const TerrainSectorMetadata& metadata)
{
	if (m_sectorMetadata.numQuadsInX != metadata.numQuadsInX)
	{
		m_sectorMetadata.numQuadsInX = metadata.numQuadsInX;
		m_dirty = true;
	}
	if (m_sectorMetadata.numQuadsInZ != metadata.numQuadsInZ)
	{
		m_sectorMetadata.numQuadsInZ = metadata.numQuadsInZ;
		m_dirty = true;
	}
	if (m_sectorMetadata.quadLenInX != metadata.numQuadsInX)
	{
		m_sectorMetadata.numQuadsInX = metadata.numQuadsInX;
		m_dirty = true;
	}
	if (m_sectorMetadata.quadLenInZ != metadata.numQuadsInZ)
	{
		m_sectorMetadata.quadLenInZ = metadata.numQuadsInZ;
		m_dirty = true;
	}
	if (m_dirty)
	{
		Regenerate();
		m_dirty = false;
	}
}

const TerrainMetadata& TerrainImpl::GetTerrainMetadata() const
{
	return m_terrainMetadata;
}

const TerrainSectorMetadata& TerrainImpl::GetSectorMetadata() const
{
	return m_sectorMetadata;
}

const std::vector<int32_t>& TerrainImpl::GetElevationMap() const
{
	return m_elevationLookup;
}

const std::vector<Mesh>& TerrainImpl::GetSectors() const
{
	return m_sectors;
}

void TerrainImpl::Regenerate()
{
	uint32_t numSectors = m_terrainMetadata.numSectorsInX * m_terrainMetadata.numSectorsInZ;
	m_sectors.clear();
	m_sectors.reserve(numSectors);
	m_textures.clear();
	m_elevationLookup.clear();
	GenerateElevationLookup();
	GenerateAllSectors();
}

void TerrainImpl::GenerateElevationLookup()
{
	const uint32_t numVerticesInX = (m_sectorMetadata.numQuadsInX * m_terrainMetadata.numSectorsInX) + 1;
	const uint32_t numVerticesInZ = (m_sectorMetadata.numQuadsInZ * m_terrainMetadata.numSectorsInZ) + 1;
	const uint32_t numVertices = numVerticesInX * numVerticesInZ;
	m_elevationLookup.resize(numVertices);	// Every vertex needs an elevation
	
	const uint32_t sectorLenInX = m_sectorMetadata.numQuadsInX * m_sectorMetadata.quadLenInX;
	const uint32_t sectorLenInZ = m_sectorMetadata.numQuadsInZ * m_sectorMetadata.quadLenInZ;
	const uint32_t terrainLenInX = sectorLenInX * m_terrainMetadata.numSectorsInX;
	const uint32_t terrainLenInZ = sectorLenInZ * m_terrainMetadata.numSectorsInZ;
	uint32_t i = 0;
	uint32_t x = 0;
	uint32_t z = 0;
	int32_t elevation = 0;
	for (uint32_t sector_row = 0; sector_row < m_terrainMetadata.numSectorsInZ; ++sector_row)
	{
		for (uint32_t sector_col = 0; sector_col < m_terrainMetadata.numSectorsInX; ++sector_col)
		{
			for (uint32_t row = 0; row < m_sectorMetadata.numQuadsInZ; ++row)
			{
				for (uint32_t col = 0; col < m_sectorMetadata.numQuadsInX; ++col)
				{
					x = sector_col * sectorLenInX + m_sectorMetadata.quadLenInX * col;
					z = sector_row * sectorLenInZ + m_sectorMetadata.quadLenInZ * row;
					elevation = static_cast<int32_t>(std::round(std::lerp(
						static_cast<float>(m_terrainMetadata.minElevation), 
						static_cast<float>(m_terrainMetadata.maxElevation), 
						GetNoiseAt(x, z, terrainLenInX, terrainLenInZ, m_terrainMetadata.redistPow, m_terrainMetadata.octaves))));
					m_elevationLookup[i++] = elevation;
				}
			}
		}
	}
	assert(i == numVertices);
}

void TerrainImpl::GenerateAllSectors()
{
	const uint32_t totalSectors = m_terrainMetadata.numSectorsInX * m_terrainMetadata.numSectorsInZ;
	m_sectors.reserve(totalSectors);
	for (uint32_t sector_row = 0; sector_row < m_terrainMetadata.numSectorsInZ; ++sector_row)
	{
		for (uint32_t sector_col = 0; sector_col < m_terrainMetadata.numSectorsInX; ++sector_col)
		{
			m_sectors.emplace_back(std::move(GenerateSectorAt(sector_row, sector_col)));
		}
	}
}

Mesh TerrainImpl::GenerateSectorAt(uint32_t sector_x, uint32_t sector_z)
{
	std::string terrainMeshName = "TerrainSector(";
	terrainMeshName += sector_x + ",";
	terrainMeshName += sector_z + ")";

	bool isUsed = false;
	const MeshID::ValueType meshHash = StringHash<MeshID::ValueType>(terrainMeshName);
	const MeshID terrainMeshID = MeshIDGenerator.AllocateID(meshHash, isUsed);
	const uint32_t numQuads = m_sectorMetadata.numQuadsInX * m_sectorMetadata.numQuadsInZ;
	const uint32_t numVertices = numQuads * 4;	// 4 vertices per quad
	const uint32_t numPolygons = numQuads * 2;	// 2 triangles per quad
	Mesh terrain(terrainMeshID, terrainMeshName.c_str(), numVertices, numPolygons);
	const uint32_t sectorLenInX = m_sectorMetadata.quadLenInX * m_sectorMetadata.numQuadsInX;
	const uint32_t sectorLenInZ = m_sectorMetadata.quadLenInZ * m_sectorMetadata.numQuadsInZ;
	
	uint32_t current_vertex_id = 0;
	uint32_t current_polygon_id = 0;
	for (uint32_t z = 0; z < static_cast<uint32_t>(m_sectorMetadata.numQuadsInZ); ++z)
	{
		for (uint32_t x = 0; x < static_cast<uint32_t>(m_sectorMetadata.numQuadsInX); ++x)
		{
			const uint32_t leftX = (sector_x * sectorLenInX) + x * m_sectorMetadata.quadLenInX;
			const uint32_t rightX = (sector_x * sectorLenInX) + (x + 1) * m_sectorMetadata.quadLenInX;
			const uint32_t bottomZ = (sector_z * sectorLenInZ) + z * m_sectorMetadata.quadLenInZ;
			const uint32_t topZ = (sector_z * sectorLenInZ) + (z + 1) * m_sectorMetadata.quadLenInZ;
			const Point bottomLeftPoint(static_cast<float>(leftX), 0.0f, static_cast<float>(bottomZ));
			const Point topLeftPoint(static_cast<float>(leftX), 0.0f,	static_cast<float>(topZ));
			const Point topRightPoint(static_cast<float>(rightX), 0.0f, static_cast<float>(topZ));
			const Point bottomRightPoint(static_cast<float>(rightX), 0.0f, static_cast<float>(bottomZ));
			const uint32_t bottomLeftPointId = current_vertex_id++;
			const uint32_t topLeftPointId = current_vertex_id++;
			const uint32_t topRightPointId = current_vertex_id++;
			const uint32_t bottomRightPointId = current_vertex_id++;
			// Position
			terrain.SetVertexPosition(bottomLeftPointId, bottomLeftPoint);
			terrain.SetVertexPosition(topLeftPointId, topLeftPoint);
			terrain.SetVertexPosition(topRightPointId, topRightPoint);
			terrain.SetVertexPosition(bottomRightPointId, bottomRightPoint);
			// UV
			terrain.SetVertexUV(0, bottomLeftPointId, UV(0.0f, 0.0f));
			terrain.SetVertexUV(0, topLeftPointId, UV(0.0f, 1.0f));
			terrain.SetVertexUV(0, topRightPointId, UV(1.0f, 1.0f));
			terrain.SetVertexUV(0, bottomRightPointId, UV(1.0f, 0.0f));
			// The two triangle indices
			terrain.SetPolygon(current_polygon_id++, VertexID(bottomLeftPointId), VertexID(topLeftPointId), VertexID(bottomRightPointId));
			terrain.SetPolygon(current_polygon_id++, VertexID(bottomRightPointId), VertexID(topLeftPointId), VertexID(topRightPointId));
		}
	}
	// Set vertex attribute
	VertexFormat meshVertexFormat;
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::Position, GetAttributeValueType<Point::ValueType>(), Point::Size);
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::Normal, GetAttributeValueType<Direction::ValueType>(), Direction::Size);
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::UV, GetAttributeValueType<UV::ValueType>(), UV::Size);
	terrain.SetVertexFormat(std::move(meshVertexFormat));

	// Set aabb
	terrain.SetAABB(AABB(
		Point(sector_x * sectorLenInX, 0, sector_z * sectorLenInZ),
		Point((sector_x + 1) * sectorLenInX, 0, (sector_z + 1) * sectorLenInZ))
	);
	return terrain;
}

}