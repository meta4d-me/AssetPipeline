#pragma once

#include "Base/Template.h"
#include "Math/AABB.hpp"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"

#include <optional>
#include <unordered_map>

namespace cd
{

class SceneDatabaseImpl
{
public:
	using TextureMap = std::unordered_map<std::string, TextureID>;

public:
	explicit SceneDatabaseImpl() = default;
	SceneDatabaseImpl(const SceneDatabaseImpl&) = default;
	SceneDatabaseImpl& operator=(const SceneDatabaseImpl&) = default;
	SceneDatabaseImpl(SceneDatabaseImpl&&) = default;
	SceneDatabaseImpl& operator=(SceneDatabaseImpl&&) = default;
	~SceneDatabaseImpl() = default;

	void SetName(std::string sceneName) { m_name = MoveTemp(sceneName); }
	const std::string& GetName() const { return m_name; }
	
	void SetAABB(AABB aabb) { m_aabb = MoveTemp(aabb); }
	AABB& GetAABB() { return m_aabb; }
	const AABB& GetAABB() const { return m_aabb; }

	// mesh
	void AddMesh(Mesh mesh);
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
	void SetMeshCount(uint32_t meshCount);
	const Mesh& GetMesh(uint32_t index) const { return m_meshes[index];  }
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshes.size()); }

	// material
	void AddMaterial(Material material);
	const std::vector<Material>& GetMaterials() const { return m_materials; }
	void SetMaterialCount(uint32_t materialCount);
	const Material& GetMaterial(uint32_t index) const { return m_materials[index]; }
	uint32_t GetMaterialCount() const { return static_cast<uint32_t>(m_materials.size()); }

	// texture
	void AddTexture(Texture texture);
	const std::vector<Texture>& GetTextures() const { return m_textures; }
	void SetTextureCount(uint32_t textureCount);
	const Texture& GetTexture(uint32_t index) const { return m_textures[index]; }
	uint32_t GetTextureCount() const { return static_cast<uint32_t>(m_textures.size()); }

	template<bool SwapBytesOrder>
	SceneDatabaseImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string sceneName;
		inputArchive >> sceneName;
		SetName(MoveTemp(sceneName));

		AABB sceneAABB;
		inputArchive >> sceneAABB;
		SetAABB(MoveTemp(sceneAABB));

		uint32_t meshCount = 0;
		uint32_t materialCount = 0;
		uint32_t textureCount = 0;
		uint32_t lightCount = 0;
		inputArchive >> meshCount >> materialCount >> textureCount >> lightCount;
		SetMeshCount(meshCount);
		SetMaterialCount(materialCount);
		SetTextureCount(textureCount);
		SetLightCount(lightCount);

		for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
		{
			AddMesh(Mesh(inputArchive));
		}

		for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
		{
			AddTexture(Texture(inputArchive));
		}

		for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			AddMaterial(Material(inputArchive));
		}

		for (uint32_t lightIndex = 0; lightIndex < lightCount; ++lightIndex)
		{
			AddLight(Light(inputArchive));
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const SceneDatabaseImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName();
		outputArchive << GetAABB();

		outputArchive << GetMeshCount() << GetMaterialCount() << GetTextureCount() << GetLightCount();

		for (uint32_t meshIndex = 0; meshIndex < GetMeshCount(); ++meshIndex)
		{
			const Mesh& mesh = GetMesh(meshIndex);
			mesh >> outputArchive;
		}

		for (uint32_t textureIndex = 0; textureIndex < GetTextureCount(); ++textureIndex)
		{
			const Texture& texture = GetTexture(textureIndex);
			texture >> outputArchive;
		}

		for (uint32_t materialIndex = 0; materialIndex < GetMaterialCount(); ++materialIndex)
		{
			const Material& material = GetMaterial(materialIndex);
			material >> outputArchive;
		}

		for (uint32_t ligthIndex = 0; ligthIndex < GetLightCount(); ++ligthIndex)
		{
			const Light& light = GetLight(ligthIndex);
			light >> outputArchive;
		}

		return *this;
	}

private:
	std::string m_name;
	AABB m_aabb;

	// mesh data
	std::vector<Mesh> m_meshes;

	// material data
	std::vector<Material> m_materials;

	// texture data
	std::vector<Texture> m_textures;
};

}