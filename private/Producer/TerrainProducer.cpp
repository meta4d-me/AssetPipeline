#include "TerrainProducer.h"

#include <cassert>
#include <climits>
#include <cmath>
#include <random>
#include <utility>

#include "Hashers/StringHash.hpp"
#include "Noise/Noise.h"
#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/MeshUtils.h"
#include "Utilities/Utils.h"

namespace cdtools
{

TerrainProducer::TerrainProducer(uint32_t x_quads, uint32_t z_quads, uint32_t quad_width, uint32_t quad_height, uint32_t max_elevation)
	: m_numQuadsInX(x_quads)
	, m_numQuadsInZ(z_quads)
	, m_quadWidth(quad_width)
	, m_quadHeight(quad_height)
	, m_maxElevation(max_elevation)
{
	// Must be at least 1 quad
	assert(x_quads >= 1);
	assert(z_quads >= 1);
	// Must be greater than zero for both width and height
	assert(quad_width > 0);
	assert(quad_height > 0);
}

void TerrainProducer::Execute(SceneDatabase* pSceneDatabase)
{
	pSceneDatabase->SetName("Generated Terrain");
	pSceneDatabase->SetMeshCount(1);

	// TODO get this from file later
	bool isUsed = false;
	pSceneDatabase->SetMaterialCount(2);
	pSceneDatabase->SetTextureCount(2);

	std::string materialName = "baseColor";
	MaterialID::ValueType materialHash = StringHash<MaterialID::ValueType>(materialName);
	MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, isUsed);
	Material baseColorMaterial(materialID, materialName.c_str());

	// TODO get this from file later
	isUsed = false;
	std::string textureName = "TerrainDirtTexture";
	TextureID::ValueType textureHash = StringHash<TextureID::ValueType>(textureName);
	TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, isUsed);
	baseColorMaterial.SetTextureID(MaterialTextureType::BaseColor, textureID);
	pSceneDatabase->AddMaterial(baseColorMaterial);
	pSceneDatabase->AddTexture(Texture(textureID, textureName.c_str()));

	isUsed = false;
	materialName = "testColor";
	materialHash = StringHash<MaterialID::ValueType>(materialName);
	MaterialID testMaterialID = m_materialIDGenerator.AllocateID(materialHash, isUsed);
	Material testColorMaterial(testMaterialID, materialName.c_str());

	// TODO get this from file later
	isUsed = false;
	textureName = "TestColorTexture";
	textureHash = StringHash<TextureID::ValueType>(textureName);
	textureID = m_textureIDGenerator.AllocateID(textureHash, isUsed);
	testColorMaterial.SetTextureID(MaterialTextureType::BaseColor, textureID);
	pSceneDatabase->AddMaterial(testColorMaterial);
	pSceneDatabase->AddTexture(Texture(textureID, textureName.c_str()));

	Mesh terrain = CreateTerrainMesh();
	terrain.SetMaterialID(materialID.Data());
	pSceneDatabase->GetAABB().Expand(terrain.GetAABB());

	// Add it to the scene
	pSceneDatabase->AddMesh(std::move(terrain));
}

Mesh TerrainProducer::CreateTerrainMesh()
{
	const uint32_t num_quads = m_numQuadsInX * m_numQuadsInZ;
	const uint32_t num_vertices = num_quads * 4;	// 4 vertices per quad
	const uint32_t num_polygons = num_quads * 2;	// 2 triangles per quad
	const std::string terrainMeshName = "generated_terrain";
	bool isUsed = false;
	MeshID::ValueType meshHash = StringHash<TextureID::ValueType>(terrainMeshName);
	MeshID terrainMeshID = m_meshIDGenerator.AllocateID(meshHash, isUsed);
	Mesh terrain(terrainMeshID, "GeneratedTerrain", num_vertices, num_polygons);

	terrain.SetVertexColorSetCount(0);	// No colors
	terrain.SetVertexUVSetCount(1);		// Only 1 set of UV
	std::vector<std::vector<TerrainQuad>> terrainQuads(m_numQuadsInZ, std::vector<TerrainQuad>(m_numQuadsInX));	// We need to store this for normal calculation later
	uint32_t current_vertex_id = 0;
	uint32_t current_polygon_id = 0;
	// Setup the parameters - TODO get this from input or a file later
	std::default_random_engine generator;
	std::uniform_int_distribution<long> distribution(LONG_MIN, LONG_MAX);
	const std::vector<std::pair<float, int64_t>> freq_params =
	{
		std::make_pair(0.0f, distribution(generator)),	// 1
		std::make_pair(0.8f, distribution(generator)),	// 2
		std::make_pair(0.8f, distribution(generator)),	// 4
		std::make_pair(0.5f, distribution(generator)),	// 8
		std::make_pair(0.4f, distribution(generator)),	// 16
		std::make_pair(0.6f, distribution(generator))	// 32
	};
	// Generate all the quads
	for (uint32_t z = 0; z < m_numQuadsInZ - 1; ++z)
	{
		std::vector<TerrainQuad>& currentRow = terrainQuads[z];
		for (uint32_t x = 0; x < m_numQuadsInX - 1; ++x)
		{
			currentRow[x] = CreateQuadAt(current_vertex_id, current_polygon_id);
			Point bottomLeftPoint(static_cast<float>(x) * m_quadWidth, GetHeightAt(x, z, freq_params, 5.0f), static_cast<float>(z) * m_quadHeight);
			Point topLeftPoint(static_cast<float>(x) * m_quadWidth, GetHeightAt(x, z + 1, freq_params, 5.0f), static_cast<float>(z + 1) * m_quadHeight);
			Point topRightPoint(static_cast<float>(x + 1) * m_quadWidth, GetHeightAt(x + 1, z + 1, freq_params, 5.0f), static_cast<float>(z + 1) * m_quadHeight);
			Point bottomRightPoint(static_cast<float>(x + 1) * m_quadWidth, GetHeightAt(x + 1, z, freq_params, 5.0f), static_cast<float>(z) * m_quadHeight);
			// Sets the attribute in the terrain
			// Position
			terrain.SetVertexPosition(currentRow[x].bottomLeftVertexId, bottomLeftPoint);
			terrain.SetVertexPosition(currentRow[x].topLeftVertexId, topLeftPoint);
			terrain.SetVertexPosition(currentRow[x].topRightVertexId, topRightPoint);
			terrain.SetVertexPosition(currentRow[x].bottomRightVertexId, bottomRightPoint);
			// UV
			terrain.SetVertexUV(0, currentRow[x].bottomLeftVertexId, UV(0.0f, 0.0f));
			terrain.SetVertexUV(0, currentRow[x].topLeftVertexId, UV(0.0f, 1.0f));
			terrain.SetVertexUV(0, currentRow[x].topRightVertexId, UV(1.0f, 1.0f));
			terrain.SetVertexUV(0, currentRow[x].bottomRightVertexId, UV(1.0f, 0.0f));
			// The two triangle indices
			terrain.SetPolygon(currentRow[x].leftTriPolygonId, VertexID(currentRow[x].bottomLeftVertexId), VertexID(currentRow[x].topLeftVertexId), VertexID(currentRow[x].bottomRightVertexId));
			terrain.SetPolygon(currentRow[x].rightTriPolygonId, VertexID(currentRow[x].bottomRightVertexId), VertexID(currentRow[x].topLeftVertexId), VertexID(currentRow[x].topRightVertexId));
			// Normals
			Direction normal;
			// bottom-left
			const Direction bottomLeftToBottomRight = bottomRightPoint - bottomLeftPoint;
			const Direction bottomLeftToTopLeft = topLeftPoint - bottomLeftPoint;
			normal = bottomLeftToBottomRight.Cross(bottomLeftToTopLeft);
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].bottomLeftVertexId, normal);
			// top-left
			const Direction topLeftToBottomLeft = bottomLeftPoint - topLeftPoint;
			const Direction topLeftToBottomRight = bottomRightPoint - topLeftPoint;
			const Direction topLeftToTopRight = topRightPoint - topLeftPoint;
			normal = topLeftToBottomLeft.Cross(topLeftToBottomRight);
			normal.Add(topLeftToBottomRight.Cross(topLeftToTopRight));
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].topLeftVertexId, normal);
			// top-right
			const Direction topRightToTopLeft = topLeftPoint - topRightPoint;
			const Direction topRightToBottomRight = bottomRightPoint - topRightPoint;
			normal = topRightToTopLeft.Cross(topRightToBottomRight);
			normal.Normalize();
			terrain.SetVertexNormal(currentRow[x].topRightVertexId, normal);
			// bottom-right
			const Direction bottomRightToTopRight = topRightPoint - bottomRightPoint;
			const Direction bottomRightToTopLeft = topLeftPoint - bottomRightPoint;
			const Direction bottomRightToBottomLeft = bottomLeftPoint - bottomRightPoint;
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
			Direction normal;
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
	VertexFormat meshVertexFormat;
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::Position, GetAttributeValueType<Point::ValueType>(), 3);
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::Normal, GetAttributeValueType<Direction::ValueType>(), 3);
	meshVertexFormat.AddAttributeLayout(VertexAttributeType::UV, GetAttributeValueType<UV::ValueType>(), 2);
	terrain.SetVertexFormat(std::move(meshVertexFormat));

	// Set aabb
	terrain.SetAABB(CalculateAABB(terrain));

	return terrain;
}

TerrainQuad TerrainProducer::CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const
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

float TerrainProducer::GetHeightAt(uint32_t x, uint32_t z, const std::vector<std::pair<float, int64_t>>& freq_params, float power_exp) const
{
	const double nx = (static_cast<double>(x) / m_numQuadsInX);
	const double nz = (static_cast<double>(z) / m_numQuadsInZ);
	float result = 0.0f;
	float scale_sum = 0.0f;
	for (uint32_t i = 0; i < freq_params.size(); ++i)
	{
		std::pair<float, int64_t> param = freq_params[i];
		const uint32_t freq  = 0x1 << i;
		result += param.first * Noise::SimplexNoise2D(param.second, freq * nx, freq * nz);
		scale_sum += param.first;
	}
	result /= scale_sum;
	result = pow(result, power_exp);
	result *= m_maxElevation;
	return result;
}

}	// namespace cdtools
