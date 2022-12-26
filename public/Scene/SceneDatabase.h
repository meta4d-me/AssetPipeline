#pragma once

#include "Base/Export.h"
#include "Math/AABB.hpp"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/Texture.h"

namespace cd
{

class SceneDatabaseImpl;

class TOOL_API SceneDatabase
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

	// mesh
	void AddMesh(Mesh mesh);
	const std::vector<Mesh>& GetMeshes() const;
	void SetMeshCount(uint32_t meshCount);
	const Mesh& GetMesh(uint32_t index) const;
	uint32_t GetMeshCount() const;

	// material
	void AddMaterial(Material material);
	const std::vector<Material>& GetMaterials() const;
	void SetMaterialCount(uint32_t materialCount);
	const Material& GetMaterial(uint32_t index) const;
	uint32_t GetMaterialCount() const;

	// texture
	void AddTexture(Texture texture);
	const std::vector<Texture>& GetTextures() const;
	void SetTextureCount(uint32_t textureCount);
	const Texture& GetTexture(uint32_t index) const;
	uint32_t GetTextureCount() const;

	SceneDatabase& operator<<(InputArchive& inputArchive);
	SceneDatabase& operator<<(InputArchiveSwapBytes& inputArchive);
	const SceneDatabase& operator>>(OutputArchive& outputArchive) const;
	const SceneDatabase& operator>>(OutputArchiveSwapBytes & outputArchive) const;

private:
	SceneDatabaseImpl* m_pSceneDatabaseImpl = nullptr;
};

}