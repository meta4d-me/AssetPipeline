#include "TerrainProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Noise/Noise.h"
#include "Producers/TerrainProducer/HeightFunctions.h"
#include "Scene/Material.h"
#include "Scene/SceneDatabase.h"
#include "Scene/Texture.h"
#include "Scene/VertexFormat.h"
#include "Utilities/MeshUtils.h"
#include "Utilities/Utils.h"

#include <format>

namespace cdtools
{

TerrainProducerImpl::TerrainProducerImpl(const TerrainGenParams& genParams)
	: m_numSectorsInX(genParams.numSectorsInX)
	, m_numSectorsInZ(genParams.numSectorsInZ)
	, m_numQuadsInSectorInX(genParams.numQuadsInSectorInX)
	, m_numQuadsInSectorInZ(genParams.numQuadsInSectorInZ)
	, m_quadLengthInX(genParams.quadLengthInX)
	, m_quadLengthInZ(genParams.quadLengthInZ)
	, m_minElevation(genParams.minElevation)
	, m_maxElevation(genParams.maxElevation)
	, m_octaves(std::move(genParams.octaves))
{
	// sanity checks
	assert(m_numSectorsInX >= 1);
	assert(m_numSectorsInZ >= 1);
	assert(m_numQuadsInSectorInX >= 1);
	assert(m_numQuadsInSectorInZ >= 1);
	assert(m_quadLengthInX >= 1);
	assert(m_quadLengthInZ >= 1);
	assert(m_minElevation <= m_maxElevation);

	// Calculate terrain dimensions
	m_sectorLengthInX = m_numQuadsInSectorInX * m_quadLengthInX;
	m_sectorLengthInZ = m_numQuadsInSectorInZ * m_quadLengthInZ;
	m_terrainLengthInX = m_numSectorsInX * m_sectorLengthInX;
	m_terrainLengthInZ = m_numSectorsInZ * m_sectorLengthInZ;

	// Print logs on what's being generated
	printf("Generating terrain with %d sectors (%d, %d). Min elevation: %d, max elevation: %d.\n", m_numSectorsInX * m_numSectorsInZ, m_numSectorsInX, m_numSectorsInZ, m_minElevation, m_maxElevation);
	printf("\tEach Sectors has %d quads (%d, %d)\n", m_numQuadsInSectorInX * m_numQuadsInSectorInZ, m_numQuadsInSectorInX, m_numQuadsInSectorInZ);
	printf("\t\tEach Quad has dimension(%d, %d)\n", m_quadLengthInX, m_quadLengthInZ);
	for (const HeightOctave& octave : m_octaves)
	{
		printf("\tOctave: seed: %I64d, freq: %f, weight: %f\n", octave.seed, octave.frequency, octave.weight);
	}
}

void TerrainProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	pSceneDatabase->SetName("Generated Terrain");
	pSceneDatabase->SetMeshCount(1);

	// TODO get this from file later
	bool isUsed = false;
	pSceneDatabase->SetMaterialCount(2);
	pSceneDatabase->SetTextureCount(2);

	std::string materialName = "baseColor";
	cd::MaterialID::ValueType materialHash = cd::StringHash<cd::MaterialID::ValueType>(materialName);
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, isUsed);
	cd::Material baseColorMaterial(materialID, materialName.c_str());

	isUsed = false;
	std::string textureName = "TerrainDirtTexture";
	cd::TextureID::ValueType textureHash = cd::StringHash<cd::TextureID::ValueType>(textureName);
	cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, isUsed);
	baseColorMaterial.SetTextureID(cd::MaterialTextureType::BaseColor, textureID);
	pSceneDatabase->AddMaterial(cd::MoveTemp(baseColorMaterial));
	pSceneDatabase->AddTexture(cd::Texture(textureID, textureName.c_str()));
	
	for (uint32_t z = 0; z < m_numSectorsInZ; ++z)
	{
		for (uint32_t x = 0; x < m_numSectorsInX; ++x)
		{
			cd::Mesh terrain = CreateTerrainMesh(x, z);
			terrain.SetMaterialID(materialID.Data());
			pSceneDatabase->GetAABB().Expand(terrain.GetAABB());

			// Add it to the scene
			pSceneDatabase->AddMesh(std::move(terrain));
		}
	}
}

cd::Mesh TerrainProducerImpl::CreateTerrainMesh(uint32_t sector_x, uint32_t sector_z)
{
	const uint32_t num_quads = m_numQuadsInSectorInX * m_numQuadsInSectorInZ;
	const uint32_t num_vertices = num_quads * 4;	// 4 vertices per quad
	const uint32_t num_polygons = num_quads * 2;	// 2 triangles per quad

	const std::string terrainMeshName = std::format("TerrainSector({},{})", sector_x, sector_z);
	bool isUsed = false;
	cd::MeshID::ValueType meshHash = cd::StringHash<cd::TextureID::ValueType>(terrainMeshName);
	cd::MeshID terrainMeshID = m_meshIDGenerator.AllocateID(meshHash, isUsed);
	cd::Mesh terrain(terrainMeshID, terrainMeshName.c_str(), num_vertices, num_polygons);

	terrain.SetVertexColorSetCount(0);	// No colors
	terrain.SetVertexUVSetCount(1);		// Only 1 set of UV
	std::vector<std::vector<TerrainQuad>> terrainQuads(m_numQuadsInSectorInZ, std::vector<TerrainQuad>(m_numQuadsInSectorInX));	// We need to store this for normal calculation later
	uint32_t current_vertex_id = 0;
	uint32_t current_polygon_id = 0;
	// Generate all the quads
	for (uint32_t z = 0; z < static_cast<uint32_t>(m_numQuadsInSectorInZ) - 1; ++z)
	{
		std::vector<TerrainQuad>& currentRow = terrainQuads[z];
		for (uint32_t x = 0; x < static_cast<uint32_t>(m_numQuadsInSectorInX) - 1; ++x)
		{
			currentRow[x] = CreateQuadAt(current_vertex_id, current_polygon_id);
			const uint32_t leftX = (sector_x * m_sectorLengthInX) + x * m_quadLengthInX;
			const uint32_t rightX = (sector_x * m_sectorLengthInX) + (x + 1) * m_quadLengthInX;
			const uint32_t bottomZ = (sector_z * m_sectorLengthInZ) + z * m_quadLengthInZ;
			const uint32_t topZ = (sector_z * m_sectorLengthInZ) + (z + 1) * m_quadLengthInZ;
			cd::Point bottomLeftPoint(
				static_cast<float>(leftX), 
				HeightFunctions::GetDefaultHeight(leftX, bottomZ, m_terrainLengthInX, m_terrainLengthInZ, m_maxElevation * 1.0f, 5.0f, m_octaves),
				static_cast<float>(bottomZ));
			cd::Point topLeftPoint(
				static_cast<float>(leftX),
				HeightFunctions::GetDefaultHeight(leftX, topZ, m_terrainLengthInX, m_terrainLengthInZ, m_maxElevation * 1.0f, 5.0f, m_octaves),
				static_cast<float>(topZ));
			cd::Point topRightPoint(
				static_cast<float>(rightX),
				HeightFunctions::GetDefaultHeight(rightX, topZ, m_terrainLengthInX, m_terrainLengthInZ, m_maxElevation * 1.0f, 5.0f, m_octaves),
				static_cast<float>(topZ));
			cd::Point bottomRightPoint(
				static_cast<float>(rightX),
				HeightFunctions::GetDefaultHeight(rightX, bottomZ, m_terrainLengthInX, m_terrainLengthInZ, m_maxElevation * 1.0f, 5.0f, m_octaves),
				static_cast<float>(bottomZ));

			// Sets the attribute in the terrain
			// Position
			terrain.SetVertexPosition(currentRow[x].bottomLeftVertexId, bottomLeftPoint);
			terrain.SetVertexPosition(currentRow[x].topLeftVertexId, topLeftPoint);
			terrain.SetVertexPosition(currentRow[x].topRightVertexId, topRightPoint);
			terrain.SetVertexPosition(currentRow[x].bottomRightVertexId, bottomRightPoint);
			// UV
			terrain.SetVertexUV(0, currentRow[x].bottomLeftVertexId, cd::UV(0.0f, 0.0f));
			terrain.SetVertexUV(0, currentRow[x].topLeftVertexId, cd::UV(0.0f, 1.0f));
			terrain.SetVertexUV(0, currentRow[x].topRightVertexId, cd::UV(1.0f, 1.0f));
			terrain.SetVertexUV(0, currentRow[x].bottomRightVertexId, cd::UV(1.0f, 0.0f));
			// The two triangle indices
			terrain.SetPolygon(currentRow[x].leftTriPolygonId, cd::VertexID(currentRow[x].bottomLeftVertexId), cd::VertexID(currentRow[x].topLeftVertexId), cd::VertexID(currentRow[x].bottomRightVertexId));
			terrain.SetPolygon(currentRow[x].rightTriPolygonId, cd::VertexID(currentRow[x].bottomRightVertexId), cd::VertexID(currentRow[x].topLeftVertexId), cd::VertexID(currentRow[x].topRightVertexId));
			// Normals
			cd::Direction normal;
			// bottom-left
			const cd::Direction bottomLeftToBottomRight = bottomRightPoint - bottomLeftPoint;
			const cd::Direction bottomLeftToTopLeft = topLeftPoint - bottomLeftPoint;
			normal = bottomLeftToBottomRight.Cross(bottomLeftToTopLeft);
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].bottomLeftVertexId, normal);
			// top-left
			const cd::Direction topLeftToBottomLeft = bottomLeftPoint - topLeftPoint;
			const cd::Direction topLeftToBottomRight = bottomRightPoint - topLeftPoint;
			const cd::Direction topLeftToTopRight = topRightPoint - topLeftPoint;
			normal = topLeftToBottomLeft.Cross(topLeftToBottomRight);
			normal.Add(topLeftToBottomRight.Cross(topLeftToTopRight));
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].topLeftVertexId, normal);
			// top-right
			const cd::Direction topRightToTopLeft = topLeftPoint - topRightPoint;
			const cd::Direction topRightToBottomRight = bottomRightPoint - topRightPoint;
			normal = topRightToTopLeft.Cross(topRightToBottomRight);
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].topRightVertexId, normal);
			// bottom-right
			const cd::Direction bottomRightToTopRight = topRightPoint - bottomRightPoint;
			const cd::Direction bottomRightToTopLeft = topLeftPoint - bottomRightPoint;
			const cd::Direction bottomRightToBottomLeft = bottomLeftPoint - bottomRightPoint;
			normal = bottomRightToTopRight.Cross(bottomRightToTopLeft);
			normal.Add(bottomRightToTopLeft.Cross(bottomRightToBottomLeft));
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].bottomRightVertexId, normal);
		}
	}
	// Smooth out all the normals by summing all the near-by quads' normals
	for (int32_t z = 0; z < terrainQuads.size(); ++z)
	{
		for (int32_t x = 0; x < terrainQuads[z].size(); ++x)
		{
			const bool hasTop = z + 1 < terrainQuads.size();
			const bool hasRight = x + 1 < terrainQuads[z].size();
			const bool hasLeft = x - 1 >= 0;
			const bool hasBottom = z - 1 >= 0;
			TerrainQuad currentQuad = terrainQuads[z][x];
			cd::Direction normal;
			// bottom-left
			normal = terrain.GetVertexNormal(currentQuad.bottomLeftVertexId);
			if (hasLeft)
			{
				const TerrainQuad leftQuad = terrainQuads[z][x - 1];
				normal.Add(terrain.GetVertexNormal(leftQuad.bottomRightVertexId));
			}
			if (hasLeft && hasBottom)
			{
				const TerrainQuad bottomLeftQuad = terrainQuads[z - 1][x - 1];
				normal.Add(terrain.GetVertexNormal(bottomLeftQuad.topRightVertexId));
			}
			if (hasBottom)
			{
				const TerrainQuad bottomQuad = terrainQuads[z - 1][x];
				normal.Add(terrain.GetVertexNormal(bottomQuad.topLeftVertexId));
			}
			normal.Normalize();
			currentQuad.bottomLeftNormal = normal;

			// top-left
			normal = terrain.GetVertexNormal(currentQuad.topLeftVertexId);
			if (hasLeft)
			{
				const TerrainQuad leftQuad = terrainQuads[z][x - 1];
				normal.Add(terrain.GetVertexNormal(leftQuad.topRightVertexId));
			}
			if (hasLeft && hasTop)
			{
				const TerrainQuad topLeftQuad = terrainQuads[z + 1][x - 1];
				normal.Add(terrain.GetVertexNormal(topLeftQuad.bottomRightVertexId));
			}
			if (hasTop)
			{
				const TerrainQuad topQuad = terrainQuads[z + 1][x];
				normal.Add(terrain.GetVertexNormal(topQuad.bottomLeftVertexId));
			}
			normal.Normalize();
			currentQuad.topRightNormal = normal;

			// top-right
			normal = terrain.GetVertexNormal(currentQuad.topRightVertexId);
			if (hasTop)
			{
				const TerrainQuad topQuad = terrainQuads[z + 1][x];
				normal.Add(terrain.GetVertexNormal(topQuad.bottomRightVertexId));
			}
			if (hasTop && hasRight)
			{
				const TerrainQuad topRightQuad = terrainQuads[z + 1][x + 1];
				normal.Add(terrain.GetVertexNormal(topRightQuad.bottomLeftVertexId));
			}
			if (hasRight)
			{
				const TerrainQuad rightQuad = terrainQuads[z][x + 1];
				normal.Add(terrain.GetVertexNormal(rightQuad.topLeftVertexId));
			}
			normal.Normalize();
			currentQuad.topRightNormal = normal;

			// bottom-right
			normal = terrain.GetVertexNormal(currentQuad.bottomRightVertexId);
			if (hasRight)
			{
				const TerrainQuad rightQuad = terrainQuads[z][x + 1];
				normal.Add(terrain.GetVertexNormal(rightQuad.bottomLeftVertexId));
			}
			if (hasRight && hasBottom)
			{
				const TerrainQuad bottomRightQuad = terrainQuads[z - 1][x + 1];
				normal.Add(terrain.GetVertexNormal(bottomRightQuad.topLeftVertexId));
			}
			if (hasBottom)
			{
				const TerrainQuad bottomQuad = terrainQuads[z - 1][x];
				normal.Add(terrain.GetVertexNormal(bottomQuad.topRightVertexId));
			}
			normal.Normalize();
			currentQuad.bottomRightNormal = normal;
		}
	}
	// Set the normal
	for (int32_t z = 0; z < terrainQuads.size(); ++z)
	{
		for (int32_t x = 0; x < terrainQuads[z].size(); ++x)
		{
			const TerrainQuad currentQuad = terrainQuads[z][x];
			terrain.SetVertexNormal(currentQuad.bottomLeftVertexId, currentQuad.bottomLeftNormal);
			terrain.SetVertexNormal(currentQuad.topLeftVertexId, currentQuad.topLeftNormal);
			terrain.SetVertexNormal(currentQuad.topRightVertexId, currentQuad.topRightNormal);
			terrain.SetVertexNormal(currentQuad.bottomRightVertexId, currentQuad.bottomRightNormal);
		}
	}

	// Set vertex attribute
	cd::VertexFormat meshVertexFormat;
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), 3);
	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), 2);
	terrain.SetVertexFormat(std::move(meshVertexFormat));

	// Set aabb
	terrain.SetAABB(CalculateAABB(terrain));

	return terrain;
}

TerrainQuad TerrainProducerImpl::CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const
{
	TerrainQuad quad;

	// Sets the vertex IDs in CW manner
	quad.bottomLeftVertexId = currentVertexId;
	++currentVertexId;
	quad.topLeftVertexId = currentVertexId;
	++currentVertexId;
	quad.topRightVertexId = currentVertexId;
	++currentVertexId;
	quad.bottomRightVertexId = currentVertexId;
	++currentVertexId;

	// Sets the polygon IDs
	quad.leftTriPolygonId = currentPolygonId;
	++currentPolygonId;
	quad.rightTriPolygonId = currentPolygonId;
	++currentPolygonId;

	return quad;
}

}	// namespace cdtools
