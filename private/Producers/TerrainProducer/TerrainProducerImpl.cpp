#include "TerrainProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Math/Math.hpp"
#include "Math/NoiseGenerator.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"
#include "Scene/Texture.h"
#include "Scene/VertexFormat.h"
#include "Utilities/StringUtils.h"
#include "Utilities/Utils.h"

#include <cinttypes>
#include <cfloat>

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

uint8_t GetChannelValue(uint32_t pixel, cdtools::AlphaMapChannel channel)
{
	// We assume the packing is RGBA in LSB order
	switch (channel)
	{
	case AlphaMapChannel::Red:
		return static_cast<uint8_t>(pixel & 0xFF);
	case AlphaMapChannel::Green:
		return static_cast<uint8_t>((pixel >> 8) & 0xFF);
	case AlphaMapChannel::Blue:
		return static_cast<uint8_t>((pixel >> 16) & 0xFF);
	case AlphaMapChannel::Alpha:
		return static_cast<uint8_t>((pixel >> 24) & 0xFF);
	default:
		assert(false);
	}
}

uint32_t PackAsRGBA8U(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	// Assume RGBA in LSB order
	return static_cast<uint32_t>(red | (green << 8) | (blue << 16) | (alpha << 24));
}

uint32_t kNextTextureId;

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
	kNextTextureId = pSceneDatabase->GetTextureCount();
}

void TerrainProducerImpl::SetTerrainMetadata(const TerrainMetadata& metadata)
{
	m_terrainMetadata = metadata;
}

void TerrainProducerImpl::SetSectorMetadata(const TerrainSectorMetadata& metadata)
{
	m_sectorMetadata = metadata;
}

void TerrainProducerImpl::SetAlphaMapTextureName(AlphaMapChannel channel, const std::string_view textureName)
{
	assert(channel != AlphaMapChannel::Count);
	m_alphaMapTextureNames[static_cast<uint8_t>(channel)] = textureName;
}

void TerrainProducerImpl::RemoveAlphaMapGeneration()
{
	m_pElevationAlphaMapDef = nullptr;
	m_pNoiseAlphaMapDef = nullptr;
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
	kNextTextureId = 0;
}

void TerrainProducerImpl::GenerateAlphaMapWithElevation(
	const AlphaMapBlendRegion<int32_t>& redGreenRegion,
	const AlphaMapBlendRegion<int32_t>& greenBlueRegion,
	const AlphaMapBlendRegion<int32_t>& blueAlphaRegion,
	const AlphaMapBlendFunction& blendFunction)
{
	m_pElevationAlphaMapDef.reset(new ElevationAlphaMapDef());
	m_pElevationAlphaMapDef->redGreenBlendRegion = redGreenRegion;
	m_pElevationAlphaMapDef->greenBlueBlendRegion = greenBlueRegion;
	m_pElevationAlphaMapDef->blueAlphaBlendRegion = blueAlphaRegion;
	m_pElevationAlphaMapDef->blendFunction = blendFunction;
	m_pNoiseAlphaMapDef = nullptr;	// only one can exist
}

void TerrainProducerImpl::Execute(SceneDatabase* pSceneDatabase)
{
	pSceneDatabase->SetName("Terrain");
	GenerateAllSectors(pSceneDatabase);
}

std::vector<std::byte> TerrainProducerImpl::GenerateElevationMap(uint32_t sector_x, uint32_t sector_z, cd::Vec2f& elevationMinMax) const
{
	std::vector<std::byte> outElevationMap;
	const uint32_t numVertices = (m_sectorLenInX + 1) * (m_sectorLenInZ + 1);
	outElevationMap.reserve(numVertices * sizeof(int32_t));

	float minElevation = FLT_MAX;
	float maxElevation = FLT_MIN;

	for (uint32_t row = 0; row <= m_sectorLenInZ; ++row)
	{
		for (uint32_t col = 0; col <= m_sectorLenInX; ++col)
		{
			const uint32_t x = (sector_x * m_sectorLenInX) + col;
			const uint32_t z = (sector_z * m_sectorLenInZ) + row;
			float elevation = std::round(
				std::lerp(
					static_cast<float>(m_terrainMetadata.minElevation),
					static_cast<float>(m_terrainMetadata.maxElevation),
					GetNoiseAt(x, z, m_terrainLenInX, m_terrainLenInZ, m_terrainMetadata.redistPow, m_terrainMetadata.octaves)));
			
			if (cd::Math::IsLargeThan(elevation, maxElevation))
			{
				maxElevation = elevation;
			}

			if (cd::Math::IsSmallThan(elevation, minElevation))
			{
				minElevation = elevation;
			}

			std::byte* bytePtr = reinterpret_cast<std::byte*>(&elevation);
			for (std::size_t i = 0; i < sizeof(int32_t); ++i)
			{
				outElevationMap.push_back(bytePtr[i]);
			}
		}
	}

	elevationMinMax.x() = minElevation;
	elevationMinMax.y() = maxElevation;

	return outElevationMap;
}

std::vector<std::byte> TerrainProducerImpl::GenerateElevationBasedAlphaMap(const std::vector<std::byte>& elevationMap) const
{
	static_assert(sizeof(int32_t) == sizeof(uint32_t));
	assert(m_pElevationAlphaMapDef != nullptr);
	assert(m_pElevationAlphaMapDef->redGreenBlendRegion.blendStart <= m_pElevationAlphaMapDef->redGreenBlendRegion.blendEnd);
	assert(m_pElevationAlphaMapDef->greenBlueBlendRegion.blendStart <= m_pElevationAlphaMapDef->greenBlueBlendRegion.blendEnd);
	assert(m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendStart <= m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendEnd);

	std::vector<std::byte> outAlphaMap;
	// We will use RGBA8 here
	// 1 byte per channel; 4 channels per pixel
	outAlphaMap.reserve(elevationMap.size());

	for (size_t elevationIndex = 0; elevationIndex < elevationMap.size() / sizeof(int32_t); ++elevationIndex)
	{
		int32_t elevation = 0;
		const std::byte* pElevationByteData = &elevationMap[elevationIndex];
		for (std::size_t byteIndex = 0; byteIndex < sizeof(int32_t); ++byteIndex)
		{
			elevation |= static_cast<int32_t>(pElevationByteData[byteIndex]) << (byteIndex * 8);
		}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
		uint8_t alpha = 0;

		// Calculate alpha map
		const float greenBlendRange = static_cast<float>(
			m_pElevationAlphaMapDef->redGreenBlendRegion.blendEnd - m_pElevationAlphaMapDef->redGreenBlendRegion.blendStart);
		const float blueBlendRange = static_cast<float>(
			m_pElevationAlphaMapDef->greenBlueBlendRegion.blendEnd - m_pElevationAlphaMapDef->greenBlueBlendRegion.blendStart);
		const float alphaBlendRange = static_cast<float>(
			m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendEnd - m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendStart);
		if (elevation < m_pElevationAlphaMapDef->redGreenBlendRegion.blendStart)
		{
			// All red
			red = 0xFF;
		}
		else if (elevation < m_pElevationAlphaMapDef->redGreenBlendRegion.blendEnd)
		{
			// Blend between red and green channel textures
			const float t = (elevation - m_pElevationAlphaMapDef->redGreenBlendRegion.blendStart) / greenBlendRange;
			switch (m_pElevationAlphaMapDef->blendFunction)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				red = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				red = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				red = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				red = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			green = static_cast<uint8_t>(0xFF - red);
		}
		else if (elevation < m_pElevationAlphaMapDef->greenBlueBlendRegion.blendStart)
		{
			// All green
			green = 0xFF;
		}
		else if (elevation < m_pElevationAlphaMapDef->greenBlueBlendRegion.blendEnd)
		{
			// Blend between green and blue channel textures
			const float t = (elevation - m_pElevationAlphaMapDef->greenBlueBlendRegion.blendStart) / blueBlendRange;
			switch (m_pElevationAlphaMapDef->blendFunction)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				green = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				green = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				green = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				green = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			blue = static_cast<uint8_t>(0xFF - green);
		}
		else if (elevation < m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendStart)
		{
			// All blue
			blue = 0xFF;
		}
		else if (elevation < m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendEnd)
		{
			// Blend between blue and alpha channel textures
			const float t = (elevation - m_pElevationAlphaMapDef->blueAlphaBlendRegion.blendStart) / alphaBlendRange;
			switch (m_pElevationAlphaMapDef->blendFunction)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				blue = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				blue = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				blue = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				blue = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			alpha = static_cast<uint8_t>(0xFF - blue);
		}
		else
		{
			// all alpha
			alpha = 0xFF;
		}
		// Write the value
		uint32_t rgba = PackAsRGBA8U(red, green, blue, alpha);

		std::byte* pRGBA = reinterpret_cast<std::byte*>(&rgba);
		for (std::size_t byteIndex = 0; byteIndex < sizeof(uint32_t); ++byteIndex)
		{
			outAlphaMap.push_back(pRGBA[byteIndex]);
		}
	}

	return outAlphaMap;
}

void TerrainProducerImpl::GenerateAllSectors(cd::SceneDatabase* pSceneDatabase)
{
	for (uint32_t sector_row = 0; sector_row < m_terrainMetadata.numSectorsInZ; ++sector_row)
	{
		for (uint32_t sector_col = 0; sector_col < m_terrainMetadata.numSectorsInX; ++sector_col)
		{
			cd::Vec2f elevationMinMax;
			std::vector<std::byte> elevationMap = GenerateElevationMap(sector_col, sector_row, elevationMinMax);
			Mesh generatedTerrain = GenerateSectorAt(sector_col, sector_row, elevationMinMax);
			MaterialID meshMaterialID = GenerateMaterialAndTextures(pSceneDatabase, sector_col, sector_row, cd::MoveTemp(elevationMap));
			generatedTerrain.SetMaterialID(meshMaterialID);
			pSceneDatabase->AddMesh(std::move(generatedTerrain));
		}
	}
}

Mesh TerrainProducerImpl::GenerateSectorAt(uint32_t sector_x, uint32_t sector_z, const cd::Vec2f& elevationMinMax)
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
			terrain.SetPolygon(current_polygon_id++, cd::Polygon(bottomLeftPointId, topLeftPointId, bottomRightPointId));
			terrain.SetPolygon(current_polygon_id++, cd::Polygon(bottomRightPointId, topLeftPointId, topRightPointId));
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
			elevationMinMax.x(),
			static_cast<float>(sector_z * m_sectorLenInZ)),
		Point(
			static_cast<float>((sector_x + 1) * m_sectorLenInX),
			elevationMinMax.y(),
			static_cast<float>((sector_z + 1) * m_sectorLenInZ))));
	return terrain;
}

MaterialID TerrainProducerImpl::GenerateMaterialAndTextures(cd::SceneDatabase* pSceneDatabase, uint32_t sector_x, uint32_t sector_z, std::vector<std::byte>&& elevationMap)
{
	const std::string materialName = string_format("TerrainMaterial(%d, %d)", sector_x, sector_z);
	MaterialID::ValueType materialHash = StringHash<MaterialID::ValueType>(materialName);
	MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash);
	Material terrainSectorMaterial(materialID, materialName.c_str(), MaterialType::BasePBR);

	// ElevationMap texture
	std::string textureName = string_format("TerrainElevationMap(%d, %d)", sector_x, sector_z);
	TextureID::ValueType textureHash = StringHash<TextureID::ValueType>(textureName);
	TextureID textureID = TextureID(kNextTextureId++);
	terrainSectorMaterial.SetTextureID(MaterialTextureType::Elevation, textureID);
	Texture elevationTexture(textureID, textureName.c_str(), MaterialTextureType::Elevation);
	elevationTexture.SetPath(textureName.c_str());
	elevationTexture.SetFormat(TextureFormat::R32I);
	elevationTexture.SetWidth(m_sectorLenInX + 1);
	elevationTexture.SetHeight(m_sectorLenInZ + 1);
	elevationTexture.SetRawData(cd::MoveTemp(elevationMap));
	pSceneDatabase->AddTexture(MoveTemp(elevationTexture));

	if (m_pElevationAlphaMapDef != nullptr)
	{
		textureName = string_format("TerrainAlphaMap(%d, %d)", sector_x, sector_z);
		textureHash = StringHash<TextureID::ValueType>(textureName);
		textureID = TextureID(kNextTextureId++);
		terrainSectorMaterial.SetTextureID(MaterialTextureType::AlphaMap, textureID);
		Texture alphaTexture(textureID, textureName.c_str(), MaterialTextureType::AlphaMap);
		alphaTexture.SetPath(textureName.c_str());
		alphaTexture.SetFormat(TextureFormat::RGBA8);
		alphaTexture.SetWidth(m_sectorLenInX + 1);
		alphaTexture.SetHeight(m_sectorLenInZ + 1);
		elevationTexture.SetRawData(GenerateElevationBasedAlphaMap(elevationMap));
		pSceneDatabase->AddTexture(MoveTemp(alphaTexture));
	}
	
	pSceneDatabase->AddMaterial(MoveTemp(terrainSectorMaterial));

	return materialID;
}

}	// namespace cdtools
