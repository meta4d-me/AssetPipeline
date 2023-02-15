#include "TerrainProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Math/NoiseGenerator.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"
#include "Scene/Texture.h"
#include "Scene/VertexFormat.h"
#include "Utilities/StringUtils.h"

#include <cinttypes>

using namespace cd;
using namespace cdtools;

namespace
{

float GetNoiseAt(
	const uint32_t x,
	const uint32_t z,
	const uint32_t terrainLenInX,
	const uint32_t terrainLenInZ,
	const float redistPower,
	const std::vector<ElevationOctave>& octaves)
{
	const double nx = x / static_cast<double>(terrainLenInX);
	const double nz = z / static_cast<double>(terrainLenInZ);
	float height = 0.0f;
	float totalWeight = 0.0f;
	for (uint32_t i = 0; i < octaves.size(); ++i)
	{
		const ElevationOctave& octave = octaves[i];
		height += octave.weight * NoiseGenerator::SimplexNoise2D(octave.seed, octave.frequency * nx, octave.frequency * nz);
		totalWeight += octave.weight;
	}
	if (totalWeight != 0.0f)
	{
		height /= totalWeight;
	}
	height = pow(height, redistPower);
	return height;
}

}

namespace cdtools
{

TerrainProducerImpl::TerrainProducerImpl(const TerrainMetadata& terrainMetadata, const TerrainSectorMetadata& sectorMetadata)
	: m_terrainMetadata(terrainMetadata)
	, m_sectorMetadata(sectorMetadata)
{
	// sanity checks
	assert(m_terrainMetadata.numSectorsInX >= 1);
	assert(m_terrainMetadata.numSectorsInZ >= 1);
	assert(m_sectorMetadata.numQuadsInX >= 1);
	assert(m_sectorMetadata.numQuadsInZ >= 1);
	assert(m_sectorMetadata.quadLenInX >= 1);
	assert(m_sectorMetadata.quadLenInZ >= 1);
	assert(m_terrainMetadata.minElevation < m_terrainMetadata.maxElevation);

	// Calculate terrain dimensions
	Initialize();
}

void TerrainProducerImpl::SetSceneDatabaseIDs(const SceneDatabase* pSceneDatabase)
{
	m_nodeIDGenerator.SetCurrentID(pSceneDatabase->GetNodeCount());
	m_meshIDGenerator.SetCurrentID(pSceneDatabase->GetMeshCount());
	m_materialIDGenerator.SetCurrentID(pSceneDatabase->GetMaterialCount());
	m_textureIDGenerator.SetCurrentID(pSceneDatabase->GetTextureCount());
}

void TerrainProducerImpl::SetTerrainMetadata(const TerrainMetadata& metadata)
{
	m_terrainMetadata = metadata;
}

void TerrainProducerImpl::SetSectorMetadata(const TerrainSectorMetadata& metadata)
{
	m_sectorMetadata = metadata;
}

void TerrainProducerImpl::Initialize()
{
	m_sectorLenInX = m_sectorMetadata.numQuadsInX * m_sectorMetadata.quadLenInX;
	m_sectorLenInZ = m_sectorMetadata.numQuadsInZ * m_sectorMetadata.quadLenInZ;
	m_sectorCount = m_terrainMetadata.numSectorsInX * m_terrainMetadata.numSectorsInZ;
	m_terrainLenInX = m_terrainMetadata.numSectorsInX * m_sectorLenInX;
	m_terrainLenInZ = m_terrainMetadata.numSectorsInZ * m_sectorLenInZ;
	m_quadsPerSector = m_sectorMetadata.numQuadsInX * m_sectorMetadata.numQuadsInZ;
	m_verticesPerSector = m_quadsPerSector * 4;
	m_trianglesPerSector = m_quadsPerSector * 2;
}

void TerrainProducerImpl::Execute(SceneDatabase* pSceneDatabase)
{
	pSceneDatabase->SetName("Terrain");
	GenerateAllSectors(pSceneDatabase);
}

void TerrainProducerImpl::GenerateElevationMap(std::vector<int32_t>& outElevationMap, uint32_t sector_x, uint32_t sector_z) const
{
	const uint32_t numVertices = (m_sectorLenInX + 1) * (m_sectorLenInZ + 1);
	outElevationMap.clear();
	outElevationMap.reserve(numVertices);
	for (uint32_t row = 0; row <= m_sectorLenInZ; ++row)
	{
		for (uint32_t col = 0; col <= m_sectorLenInX; ++col)
		{
			const uint32_t x = (sector_x * m_sectorLenInX) + col;
			const uint32_t z = (sector_z * m_sectorLenInZ) + row;
			outElevationMap.push_back(static_cast<int32_t>(
				std::round(
					std::lerp(
						static_cast<float>(m_terrainMetadata.minElevation),
						static_cast<float>(m_terrainMetadata.maxElevation),
						GetNoiseAt(x, z, m_terrainLenInX, m_terrainLenInZ, m_terrainMetadata.redistPow, m_terrainMetadata.octaves)
					)
				)
				));
		}
	}
}

void TerrainProducerImpl::GenerateAllSectors(cd::SceneDatabase* pSceneDatabase)
{
	std::vector<int32_t> elevationMap;
	for (uint32_t sector_row = 0; sector_row < m_terrainMetadata.numSectorsInZ; ++sector_row)
	{
		for (uint32_t sector_col = 0; sector_col < m_terrainMetadata.numSectorsInX; ++sector_col)
		{
			GenerateElevationMap(elevationMap, sector_col, sector_row);
			Mesh generatedTerrain = GenerateSectorAt(sector_col, sector_row, elevationMap);
			MaterialID meshMaterialID = GenerateMaterialAndTextures(pSceneDatabase, sector_col, sector_row, elevationMap);
			generatedTerrain.SetMaterialID(meshMaterialID.Data());
			pSceneDatabase->AddMesh(std::move(generatedTerrain));
		}
	}
}

Mesh TerrainProducerImpl::GenerateSectorAt(uint32_t sector_x, uint32_t sector_z, const std::vector<int32_t>& elevationMap)
{
	const std::string terrainMeshName = string_format("TerrainSector(%d, %d)", sector_x, sector_z);
	const MeshID::ValueType meshHash = StringHash<MeshID::ValueType>(terrainMeshName);
	const MeshID terrainMeshID = m_meshIDGenerator.AllocateID(meshHash);
	Mesh terrain(terrainMeshID, terrainMeshName.c_str(), m_verticesPerSector, m_trianglesPerSector);
	terrain.SetVertexUVSetCount(1);

	uint32_t current_vertex_id = 0;
	uint32_t current_polygon_id = 0;
	for (uint32_t z = 0; z < static_cast<uint32_t>(m_sectorMetadata.numQuadsInZ); ++z)
	{
		for (uint32_t x = 0; x < static_cast<uint32_t>(m_sectorMetadata.numQuadsInX); ++x)
		{
			const uint32_t leftX = (sector_x * m_sectorLenInX) + x * m_sectorMetadata.quadLenInX;
			const uint32_t rightX = (sector_x * m_sectorLenInX) + (x + 1) * m_sectorMetadata.quadLenInX;
			const uint32_t bottomZ = (sector_z * m_sectorLenInZ) + z * m_sectorMetadata.quadLenInZ;
			const uint32_t topZ = (sector_z * m_sectorLenInZ) + (z + 1) * m_sectorMetadata.quadLenInZ;
			const Point bottomLeftPoint(
				static_cast<float>(leftX), 
				static_cast<float>(m_terrainMetadata.minElevation), 
				static_cast<float>(bottomZ));
			const Point topLeftPoint(
				static_cast<float>(leftX), 
				static_cast<float>(m_terrainMetadata.minElevation), 
				static_cast<float>(topZ));
			const Point topRightPoint(
				static_cast<float>(rightX), 
				static_cast<float>(m_terrainMetadata.minElevation), 
				static_cast<float>(topZ));
			const Point bottomRightPoint(
				static_cast<float>(rightX), 
				static_cast<float>(m_terrainMetadata.minElevation), 
				static_cast<float>(bottomZ));
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
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::UV, GetAttributeValueType<UV::ValueType>(), UV::Size);
	terrain.SetVertexFormat(std::move(meshVertexFormat));

	// Set aabb
	terrain.SetAABB(AABB(
		Point(
			static_cast<float>(sector_x * m_sectorLenInX),
			static_cast<float>(*std::min_element(elevationMap.begin(), elevationMap.end())),
			static_cast<float>(sector_z * m_sectorLenInZ)),
		Point(
			static_cast<float>((sector_x + 1) * m_sectorLenInX),
			static_cast<float>(*std::max_element(elevationMap.begin(), elevationMap.end())),
			static_cast<float>((sector_z + 1) * m_sectorLenInZ))));
	return terrain;
}

MaterialID TerrainProducerImpl::GenerateMaterialAndTextures(cd::SceneDatabase* pSceneDatabase, uint32_t sector_x, uint32_t sector_z, std::vector<int32_t>& elevationMap)
{
	const std::string materialName = string_format("TerrainMaterial(%d, %d)", sector_x, sector_z);
	MaterialID::ValueType materialHash = StringHash<MaterialID::ValueType>(materialName);
	MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash);
	Material terrainSectorMaterial(materialID, materialName.c_str(), MaterialType::BasePBR);

	// Base color texture
	std::string textureName = "Terrain_baseColor";
	TextureID::ValueType textureHash = StringHash<TextureID::ValueType>(textureName);
	TextureID textureID = m_textureIDGenerator.AllocateID(textureHash);
	terrainSectorMaterial.AddTextureID(MaterialTextureType::BaseColor, textureID);
	pSceneDatabase->AddTexture(Texture(textureID, MaterialTextureType::BaseColor, textureName.c_str()));

	// ElevationMap texture
	textureName = string_format("TerrainElevationMap(%d, %d)", sector_x, sector_z);
	textureHash = StringHash<TextureID::ValueType>(textureName);
	textureID = m_textureIDGenerator.AllocateID(textureHash);
	terrainSectorMaterial.AddTextureID(MaterialTextureType::Roughness, textureID);
	Texture elevationTexture = Texture(textureID, MaterialTextureType::Roughness, textureName.c_str());
	elevationTexture.SetRawTexture(elevationMap, TextureFormat::R32I, m_sectorLenInX + 1, m_sectorLenInZ + 1);
	pSceneDatabase->AddTexture(MoveTemp(elevationTexture));
	
	pSceneDatabase->AddMaterial(MoveTemp(terrainSectorMaterial));

	return materialID;
}

}	// namespace cdtools
