#pragma once

#include "Base/Endian.h"
#include "Base/Export.h"
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

	// Identify
	void SetName(const char* pName);
	const char* GetName() const;
	
	// Volumn
	void SetAABB(AABB aabb);
	AABB& GetAABB();
	const AABB& GetAABB() const;

	// AxisSystem
	void SetAxisSystem(AxisSystem axis);
	AxisSystem& GetAxisSystem();
	const AxisSystem& GetAxisSystem() const;

	// Unit
	void SetUnit(Unit unit);
	Unit& GetUnit();
	Unit GetUnit() const;

	// Node
	void AddNode(Node node);
	std::vector<Node>& GetNodes();
	const std::vector<Node>& GetNodes() const;
	void SetNodeCount(uint32_t nodeCount);
	const Node& GetNode(uint32_t index) const;
	const Node* GetNodeByName(const char* pName) const;
	uint32_t GetNodeCount() const;

	// Mesh
	void AddMesh(Mesh mesh);
	std::vector<Mesh>& GetMeshes();
	const std::vector<Mesh>& GetMeshes() const;
	void SetMeshCount(uint32_t meshCount);
	const Mesh& GetMesh(uint32_t index) const;
	uint32_t GetMeshCount() const;

	// Morph
	void AddMorph(Morph morph);
	std::vector<Morph>& GetMorphs();
	const std::vector<Morph>& GetMorphs() const;
	void SetMorphCount(uint32_t morphCount);
	const Morph& GetMorph(uint32_t index) const;
	uint32_t GetMorphCount() const;

	// Material
	void AddMaterial(Material material);
	std::vector<Material>& GetMaterials();
	const std::vector<Material>& GetMaterials() const;
	void SetMaterialCount(uint32_t materialCount);
	const Material& GetMaterial(uint32_t index) const;
	uint32_t GetMaterialCount() const;

	// Texture
	void AddTexture(Texture texture);
	std::vector<Texture>& GetTextures();
	const std::vector<Texture>& GetTextures() const;
	void SetTextureCount(uint32_t textureCount);
	const Texture& GetTexture(uint32_t index) const;
	uint32_t GetTextureCount() const;

	// Camera
	void AddCamera(Camera camera);
	std::vector<Camera>& GetCameras();
	const std::vector<Camera>& GetCameras() const;
	void SetCameraCount(uint32_t cameraCount);
	const Camera& GetCamera(uint32_t index) const;
	uint32_t GetCameraCount() const;

	// Light
	void AddLight(Light light);
	std::vector<Light>& GetLights();
	const std::vector<Light>& GetLights() const;
	void SetLightCount(uint32_t lightCount);
	const Light& GetLight(uint32_t index) const;
	uint32_t GetLightCount() const;

	// Bone
	void AddBone(Bone bone);
	std::vector<Bone>& GetBones();
	const std::vector<Bone>& GetBones() const;
	void SetBoneCount(uint32_t boneCount);
	const Bone& GetBone(uint32_t index) const;
	const Bone* GetBoneByName(const char* pName) const;
	uint32_t GetBoneCount() const;

	// Animation
	void AddAnimation(Animation animation);
	std::vector<Animation>& GetAnimations();
	const std::vector<Animation>& GetAnimations() const;
	void SetAnimationCount(uint32_t animationCount);
	const Animation& GetAnimation(uint32_t index) const;
	uint32_t GetAnimationCount() const;

	// Track
	void AddTrack(Track Track);
	std::vector<Track>& GetTracks();
	const std::vector<Track>& GetTracks() const;
	void SetTrackCount(uint32_t TrackCount);
	const Track& GetTrack(uint32_t index) const;
	const Track* GetTrackByName(const char* pName) const;
	uint32_t GetTrackCount() const;

	void UpdateAABB();

	// Serialization
	SceneDatabase& operator<<(InputArchive& inputArchive);
	SceneDatabase& operator<<(InputArchiveSwapBytes& inputArchive);
	const SceneDatabase& operator>>(OutputArchive& outputArchive) const;
	const SceneDatabase& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	SceneDatabaseImpl* m_pSceneDatabaseImpl = nullptr;
};

}