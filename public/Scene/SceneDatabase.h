#pragma once

#include "Base/Endian.h"
#include "Base/Export.h"
#include "Base/Template.h"
#include "Math/Box.hpp"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/Node.h"
#include "Scene/Texture.h"
#include "Scene/Light.h"

#include <vector>

namespace cd
{

class SceneDatabaseImpl;

class CORE_API SceneDatabase final
{
public:
	explicit SceneDatabase();
	SceneDatabase(const SceneDatabase&) = default;
	SceneDatabase& operator=(const SceneDatabase&) = default;
	SceneDatabase(SceneDatabase&&);
	SceneDatabase& operator=(SceneDatabase&&);
	~SceneDatabase();

	void SetName(const char* pName);
	const char* GetName() const;
	
	void SetAABB(AABB aabb);
	AABB& GetAABB();
	const AABB& GetAABB() const;

	// Node
	void AddNode(Node node);
	const std::vector<Node>& GetNodes() const;
	void SetNodeCount(uint32_t nodeCount);
	const Node& GetNode(uint32_t index) const;
	uint32_t GetNodeCount() const;

	// Mesh
	void AddMesh(Mesh mesh);
	const std::vector<Mesh>& GetMeshes() const;
	void SetMeshCount(uint32_t meshCount);
	const Mesh& GetMesh(uint32_t index) const;
	uint32_t GetMeshCount() const;

	// Material
	void AddMaterial(Material material);
	const std::vector<Material>& GetMaterials() const;
	void SetMaterialCount(uint32_t materialCount);
	const Material& GetMaterial(uint32_t index) const;
	uint32_t GetMaterialCount() const;

	// Texture
	void AddTexture(Texture texture);
	const std::vector<Texture>& GetTextures() const;
	void SetTextureCount(uint32_t textureCount);
	const Texture& GetTexture(uint32_t index) const;
	uint32_t GetTextureCount() const;

	// Light
	void AddLight(Light light);
	const std::vector<Light>& GetLights() const;
	void SetLightCount(uint32_t lightCount);
	const Light& GetLight(uint32_t index) const;
	uint32_t GetLightCount() const;

	// Serialization
	SceneDatabase& operator<<(InputArchive& inputArchive);
	SceneDatabase& operator<<(InputArchiveSwapBytes& inputArchive);
	const SceneDatabase& operator>>(OutputArchive& outputArchive) const;
	const SceneDatabase& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	SceneDatabaseImpl* m_pSceneDatabaseImpl = nullptr;
};

}