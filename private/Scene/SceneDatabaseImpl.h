#pragma once

#include "Base/Template.h"
#include "Math/Box.hpp"
#include "Math/UnitSystem.hpp"
#include "Scene/Animation.h"
#include "Scene/Bone.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/Morph.h"
#include "Scene/Node.h"
#include "Scene/Texture.h"
#include "Scene/Track.h"

#include <optional>
#include <unordered_map>

namespace cd
{

class SceneDatabaseImpl
{
public:
	using TextureMap = std::unordered_map<std::string, TextureID>;

public:
	SceneDatabaseImpl();
	SceneDatabaseImpl(const SceneDatabaseImpl&) = default;
	SceneDatabaseImpl& operator=(const SceneDatabaseImpl&) = default;
	SceneDatabaseImpl(SceneDatabaseImpl&&) = default;
	SceneDatabaseImpl& operator=(SceneDatabaseImpl&&) = default;
	~SceneDatabaseImpl() = default;

	// Identity
	void SetName(std::string sceneName) { m_name = MoveTemp(sceneName); }
	const std::string& GetName() const { return m_name; }
	
	// Volumn
	void SetAABB(AABB aabb) { m_aabb = MoveTemp(aabb); }
	AABB& GetAABB() { return m_aabb; }
	const AABB& GetAABB() const { return m_aabb; }

	// AxisSystem
	void SetAxisSystem(AxisSystem axis) { m_axisSystem = MoveTemp(axis); }
	AxisSystem& GetAxisSystem() { return m_axisSystem; }
	const AxisSystem& GetAxisSystem() const { return m_axisSystem; }

	// Unit
	void SetUnit(Unit unit) { m_unit = unit; }
	Unit& GetUnit() { return m_unit; }
	Unit GetUnit() const { return m_unit; }

	// Node
	void AddNode(Node node) { m_nodes.emplace_back(MoveTemp(node)); }
	std::vector<Node>& GetNodes() { return m_nodes; }
	const std::vector<Node>& GetNodes() const { return m_nodes; }
	void SetNodeCount(uint32_t count) { m_nodes.reserve(count); }
	Node& GetNode(uint32_t index) { return m_nodes[index]; }
	const Node& GetNode(uint32_t index) const { return m_nodes[index]; }
	const Node* GetNodeByName(const char* pName) const;
	uint32_t GetNodeCount() const { return static_cast<uint32_t>(m_nodes.size()); }

	// Mesh
	void AddMesh(Mesh mesh) { m_meshes.emplace_back(MoveTemp(mesh)); }
	std::vector<Mesh>& GetMeshes() { return m_meshes; }
	const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
	void SetMeshCount(uint32_t count) { m_meshes.reserve(count); }
	const Mesh& GetMesh(uint32_t index) const { return m_meshes[index];  }
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshes.size()); }

	// Morph
	void AddMorph(Morph morph) { m_morphs.emplace_back(MoveTemp(morph)); }
	std::vector<Morph>& GetMorphs() { return m_morphs; }
	const std::vector<Morph>& GetMorphs() const { return m_morphs; }
	void SetMorphCount(uint32_t count) { m_morphs.reserve(count); }
	const Morph& GetMorph(uint32_t index) const { return m_morphs[index]; }
	uint32_t GetMorphCount() const { return static_cast<uint32_t>(m_morphs.size()); }

	// Texture
	void AddTexture(Texture texture) { m_textures.emplace_back(MoveTemp(texture)); }
	std::vector<Texture>& GetTextures() { return m_textures; }
	const std::vector<Texture>& GetTextures() const { return m_textures; }
	void SetTextureCount(uint32_t count) { m_textures.reserve(count); }
	Texture& GetTexture(uint32_t index) { return m_textures[index]; }
	const Texture& GetTexture(uint32_t index) const { return m_textures[index]; }
	uint32_t GetTextureCount() const { return static_cast<uint32_t>(m_textures.size()); }

	// Material
	void AddMaterial(Material material) { m_materials.emplace_back(MoveTemp(material)); }
	std::vector<Material>& GetMaterials() { return m_materials; }
	const std::vector<Material>& GetMaterials() const { return m_materials; }
	void SetMaterialCount(uint32_t count) { m_materials.reserve(count); }
	const Material& GetMaterial(uint32_t index) const { return m_materials[index]; }
	uint32_t GetMaterialCount() const { return static_cast<uint32_t>(m_materials.size()); }

	// Camera
	void AddCamera(Camera camera) { m_cameras.emplace_back(MoveTemp(camera)); }
	std::vector<Camera>& GetCameras() { return m_cameras; }
	const std::vector<Camera>& GetCameras() const { return m_cameras; }
	void SetCameraCount(uint32_t count) { m_cameras.reserve(count); }
	const Camera& GetCamera(uint32_t index) const { return m_cameras[index]; }
	uint32_t GetCameraCount() const { return static_cast<uint32_t>(m_cameras.size()); }

	// Light
	void AddLight(Light light) { m_lights.emplace_back(MoveTemp(light)); }
	std::vector<Light>& GetLights() { return m_lights; }
	const std::vector<Light>& GetLights() const { return m_lights; }
	void SetLightCount(uint32_t count) { m_lights.reserve(count); }
	const Light& GetLight(uint32_t index) const { return m_lights[index]; }
	uint32_t GetLightCount() const { return static_cast<uint32_t>(m_lights.size()); }

	// Bone
	void AddBone(Bone bone) { m_bones.emplace_back(MoveTemp(bone)); }
	std::vector<Bone>& GetBones() { return m_bones; }
	const std::vector<Bone>& GetBones() const { return m_bones; }
	void SetBoneCount(uint32_t count) { m_bones.reserve(count); }
	const Bone& GetBone(uint32_t index) const { return m_bones[index]; }
	const Bone* GetBoneByName(const char* pName) const;
	uint32_t GetBoneCount() const { return static_cast<uint32_t>(m_bones.size()); }

	// Animation
	void AddAnimation(Animation animation) { m_animations.emplace_back(MoveTemp(animation)); }
	std::vector<Animation>& GetAnimations() { return m_animations; }
	const std::vector<Animation>& GetAnimations() const { return m_animations; }
	void SetAnimationCount(uint32_t count) { m_animations.reserve(count); }
	const Animation& GetAnimation(uint32_t index) const { return m_animations[index]; }
	uint32_t GetAnimationCount() const { return static_cast<uint32_t>(m_animations.size()); }

	// Track
	void AddTrack(Track Track) { m_tracks.emplace_back(MoveTemp(Track)); }
	std::vector<Track>& GetTracks() { return m_tracks; }
	const std::vector<Track>& GetTracks() const { return m_tracks; }
	void SetTrackCount(uint32_t count) { m_tracks.reserve(count); }
	const Track& GetTrack(uint32_t index) const { return m_tracks[index]; }
	const Track* GetTrackByName(const char* pName) const;
	uint32_t GetTrackCount() const { return static_cast<uint32_t>(m_tracks.size()); }

	void Dump() const;
	void Validate() const;
	void Merge(cd::SceneDatabaseImpl&& sceneDatabaseImpl);
	void UpdateAABB();

	template<bool SwapBytesOrder>
	SceneDatabaseImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string sceneName;
		inputArchive >> sceneName;
		SetName(MoveTemp(sceneName));

		AABB sceneAABB;
		inputArchive >> sceneAABB;
		SetAABB(MoveTemp(sceneAABB));

		AxisSystem axisSystem;
		inputArchive >> axisSystem;
		SetAxisSystem(MoveTemp(axisSystem));

		uint8_t unit;
		inputArchive >> unit;
		SetUnit(static_cast<Unit>(unit));

		uint32_t nodeCount;
		uint32_t meshCount;
		uint32_t textureCount;
		uint32_t materialCount;
		uint32_t cameraCount;
		uint32_t lightCount;
		uint32_t boneCount;
		uint32_t animationCount;
		uint32_t trackCount;
		uint32_t morphCount;

		inputArchive >> nodeCount >> meshCount >> morphCount
			>> materialCount >> textureCount
			>> cameraCount >> lightCount
			>> boneCount >> animationCount >> trackCount;

		SetNodeCount(nodeCount);
		SetMeshCount(meshCount);
		SetMorphCount(morphCount);
		SetMaterialCount(materialCount);
		SetTextureCount(textureCount);
		SetCameraCount(cameraCount);
		SetLightCount(lightCount);
		SetBoneCount(boneCount);
		SetAnimationCount(animationCount);
		SetTrackCount(trackCount);

		for (uint32_t nodeIndex = 0U; nodeIndex < nodeCount; ++nodeIndex)
		{
			AddNode(Node(inputArchive));
		}

		for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
		{
			AddMesh(Mesh(inputArchive));
		}

		for (uint32_t morphIndex = 0U; morphIndex < morphCount; ++morphIndex)
		{
			AddMorph(Morph(inputArchive));
		}

		for (uint32_t materialIndex = 0U; materialIndex < materialCount; ++materialIndex)
		{
			AddMaterial(Material(inputArchive));
		}

		for (uint32_t textureIndex = 0U; textureIndex < textureCount; ++textureIndex)
		{
			AddTexture(Texture(inputArchive));
		}

		for (uint32_t cameraIndex = 0U; cameraIndex < cameraCount; ++cameraIndex)
		{
			AddCamera(Camera(inputArchive));
		}

		for (uint32_t lightIndex = 0U; lightIndex < lightCount; ++lightIndex)
		{
			AddLight(Light(inputArchive));
		}

		for (uint32_t boneIndex = 0U; boneIndex < boneCount; ++boneIndex)
		{
			AddBone(Bone(inputArchive));
		}

		for (uint32_t animationIndex = 0U; animationIndex < animationCount; ++animationIndex)
		{
			AddAnimation(Animation(inputArchive));
		}

		for (uint32_t trackIndex = 0U; trackIndex < trackCount; ++trackIndex)
		{
			AddTrack(Track(inputArchive));
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const SceneDatabaseImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetAABB() << GetAxisSystem() << static_cast<uint8_t>(GetUnit())
			<< GetNodeCount() << GetMeshCount() << GetMorphCount()
			<< GetMaterialCount() << GetTextureCount()
			<< GetCameraCount() << GetLightCount()
			<< GetBoneCount() << GetAnimationCount() << GetTrackCount();

		for (uint32_t nodeIndex = 0U; nodeIndex < GetNodeCount(); ++nodeIndex)
		{
			const Node& node = GetNode(nodeIndex);
			node >> outputArchive;
		}

		for (uint32_t meshIndex = 0U; meshIndex < GetMeshCount(); ++meshIndex)
		{
			const Mesh& mesh = GetMesh(meshIndex);
			mesh >> outputArchive;
		}

		for (uint32_t morphIndex = 0U; morphIndex < GetMorphCount(); ++morphIndex)
		{
			const Morph& morph = GetMorph(morphIndex);
			morph >> outputArchive;
		}

		for (uint32_t materialIndex = 0U; materialIndex < GetMaterialCount(); ++materialIndex)
		{
			const Material& material = GetMaterial(materialIndex);
			material >> outputArchive;
		}

		for (uint32_t textureIndex = 0U; textureIndex < GetTextureCount(); ++textureIndex)
		{
			const Texture& texture = GetTexture(textureIndex);
			texture >> outputArchive;
		}

		for (uint32_t cameraIndex = 0U; cameraIndex < GetCameraCount(); ++cameraIndex)
		{
			const Camera& camera = GetCamera(cameraIndex);
			camera >> outputArchive;
		}

		for (uint32_t ligthIndex = 0U; ligthIndex < GetLightCount(); ++ligthIndex)
		{
			const Light& light = GetLight(ligthIndex);
			light >> outputArchive;
		}

		for (uint32_t boneIndex = 0U; boneIndex < GetBoneCount(); ++boneIndex)
		{
			const Bone& bone = GetBone(boneIndex);
			bone >> outputArchive;
		}

		for (uint32_t animationIndex = 0U; animationIndex < GetAnimationCount(); ++animationIndex)
		{
			const Animation& animation = GetAnimation(animationIndex);
			animation >> outputArchive;
		}

		for (uint32_t trackIndex = 0U; trackIndex < GetTrackCount(); ++trackIndex)
		{
			const Track& track = GetTrack(trackIndex);
			track >> outputArchive;
		}

		return *this;
	}

private:
	std::string m_name;
	AABB m_aabb;
	AxisSystem m_axisSystem;
	Unit m_unit;

	// Hierarchy data to present relationships between meshes.
	std::vector<Node> m_nodes;

	// Mesh data both for StatciMesh and SkinMesh.
	std::vector<Mesh> m_meshes;
	std::vector<Morph> m_morphs;

	// Texturing data.
	std::vector<Material> m_materials;
	std::vector<Texture> m_textures;

	// Scene objects.
	std::vector<Camera> m_cameras;
	std::vector<Light> m_lights;

	// Animation data.
	std::vector<Bone> m_bones;
	std::vector<Animation> m_animations;
	std::vector<Track> m_tracks;
};

}