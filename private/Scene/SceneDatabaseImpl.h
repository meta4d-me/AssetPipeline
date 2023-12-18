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
#include "Scene/Skin.h"
#include "Scene/Skeleton.h"
#include "Scene/Texture.h"
#include "Scene/Track.h"
#include "Scene/ParticleEmitter.h"

#include <optional>
#include <unordered_map>
#include <vector>

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

	IMPLEMENT_SIMPLE_TYPE_APIS(SceneDatabase, Unit);
	IMPLEMENT_COMPLEX_TYPE_APIS(SceneDatabase, AABB);
	IMPLEMENT_COMPLEX_TYPE_APIS(SceneDatabase, AxisSystem);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Animation);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Bone);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Camera);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Light);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Material);
	IMPLEMENT_VECTOR_TYPE_APIS_WITH_PLURAL(SceneDatabase, Mesh, es);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Morph);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Node);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, ParticleEmitter);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Skeleton);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Skin);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Texture);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Track);
	IMPLEMENT_STRING_TYPE_APIS(SceneDatabase, Name);

	// Bone
	Bone* GetBoneByName(const char* pName);
	const Bone* GetBoneByName(const char* pName) const;

	// Node
	Node* GetNodeByName(const char* pName);
	const Node* GetNodeByName(const char* pName) const;

	// Track
	Track* GetTrackByName(const char* pName);
	const Track* GetTrackByName(const char* pName) const;

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
		uint32_t skinCount;
		uint32_t skeletonCount;
		uint32_t boneCount;
		uint32_t animationCount;
		uint32_t trackCount;
		uint32_t morphCount;
		uint32_t particleEmitterCount;

		inputArchive >> nodeCount >> meshCount >> morphCount
			>> materialCount >> textureCount
			>> cameraCount >> lightCount
			>> skinCount >> skeletonCount >> boneCount
			>> animationCount >> trackCount
			>> particleEmitterCount;

		SetNodeCount(nodeCount);
		SetMeshCount(meshCount);
		SetMorphCount(morphCount);
		SetMaterialCount(materialCount);
		SetTextureCount(textureCount);
		SetCameraCount(cameraCount);
		SetLightCount(lightCount);
		SetSkinCount(skinCount);
		SetSkeletonCount(skeletonCount);
		SetBoneCount(boneCount);
		SetAnimationCount(animationCount);
		SetTrackCount(trackCount);
		SetParticleEmitterCount(particleEmitterCount);

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

		for (uint32_t skinIndex = 0U; skinIndex < skinCount; ++skinIndex)
		{
			AddSkin(Skin(inputArchive));
		}

		for (uint32_t skeletonIndex = 0U; skeletonIndex < skeletonCount; ++skeletonIndex)
		{
			AddSkeleton(Skeleton(inputArchive));
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

		for (uint32_t particleIndex = 0U; particleIndex < particleEmitterCount; ++particleIndex)
		{
			AddParticleEmitter(ParticleEmitter(inputArchive));
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
			<< GetSkinCount() << GetSkeletonCount() << GetBoneCount()
			<< GetAnimationCount() << GetTrackCount()
			<< GetParticleEmitterCount();

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

		for (uint32_t skinIndex = 0U; skinIndex < GetSkinCount(); ++skinIndex)
		{
			const Skin& skin = GetSkin(skinIndex);
			skin >> outputArchive;
		}

		for (uint32_t skeletonIndex = 0U; skeletonIndex < GetSkeletonCount(); ++skeletonIndex)
		{
			const Skeleton& skeleton = GetSkeleton(skeletonIndex);
			skeleton >> outputArchive;
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

		for (uint32_t particleIndex = 0U; particleIndex < GetParticleEmitterCount(); ++particleIndex)
		{
			const ParticleEmitter& particle = GetParticleEmitter(particleIndex);
			particle >> outputArchive;
		}

		return *this;
	}
};

}