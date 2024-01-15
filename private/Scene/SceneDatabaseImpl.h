#pragma once

#include "Base/Template.h"
#include "Math/Box.hpp"
#include "Math/UnitSystem.hpp"
#include "Scene/Animation.h"
#include "Scene/BlendShape.h"
#include "Scene/Bone.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/Morph.h"
#include "Scene/Node.h"
#include "Scene/Skeleton.h"
#include "Scene/Skin.h"
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
	IMPLEMENT_STRING_TYPE_APIS(SceneDatabase, Name);
	IMPLEMENT_COMPLEX_TYPE_APIS(SceneDatabase, AABB);
	IMPLEMENT_COMPLEX_TYPE_APIS(SceneDatabase, AxisSystem);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Animation);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, BlendShape);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Bone);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Camera);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Light);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Material);
	IMPLEMENT_VECTOR_TYPE_APIS_WITH_PLURAL(SceneDatabase, Mesh, es);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Morph);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Node);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, RootNodeID);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, ParticleEmitter);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Skeleton);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Skin);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Texture);
	IMPLEMENT_VECTOR_TYPE_APIS(SceneDatabase, Track);

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

		uint32_t rootNodeIDCount;
		uint32_t nodeCount;
		uint32_t meshCount;
		uint32_t blendShapeCount;
		uint32_t morphCount;
		uint32_t materialCount;
		uint32_t textureCount;
		uint32_t cameraCount;
		uint32_t lightCount;
		uint32_t skinCount;
		uint32_t skeletonCount;
		uint32_t boneCount;
		uint32_t animationCount;
		uint32_t trackCount;
		uint32_t particleEmitterCount;

		inputArchive >> rootNodeIDCount >> nodeCount
			>> meshCount >> blendShapeCount >> morphCount
			>> materialCount >> textureCount
			>> cameraCount >> lightCount
			>> skinCount >> skeletonCount >> boneCount
			>> animationCount >> trackCount
			>> particleEmitterCount;

		SetRootNodeIDCapacity(rootNodeIDCount);
		SetNodeCapacity(nodeCount);
		SetMeshCapacity(meshCount);
		SetBlendShapeCapacity(blendShapeCount);
		SetMorphCapacity(morphCount);
		SetMaterialCapacity(materialCount);
		SetTextureCapacity(textureCount);
		SetCameraCapacity(cameraCount);
		SetLightCapacity(lightCount);
		SetSkeletonCapacity(boneCount);
		SetBoneCapacity(boneCount);
		SetAnimationCapacity(animationCount);
		SetTrackCapacity(trackCount);
		SetParticleEmitterCapacity(particleEmitterCount);

		inputArchive.ImportBuffer(GetRootNodeIDs().data());
		
		for (uint32_t nodeIndex = 0U; nodeIndex < nodeCount; ++nodeIndex)
		{
			AddNode(Node(inputArchive));
		}

		for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
		{
			AddMesh(Mesh(inputArchive));
		}

		for (uint32_t blendShapeIndex = 0U; blendShapeIndex < blendShapeCount; ++blendShapeIndex)
		{
			AddBlendShape(BlendShape(inputArchive));
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
			<< GetRootNodeIDCount() << GetNodeCount()
			<< GetMeshCount() << GetBlendShapeCount() << GetMorphCount()
			<< GetMaterialCount() << GetTextureCount()
			<< GetCameraCount() << GetLightCount()
			<< GetSkinCount() << GetSkeletonCount() << GetBoneCount()
			<< GetAnimationCount() << GetTrackCount()
			<< GetParticleEmitterCount();

		outputArchive.ExportBuffer(GetRootNodeIDs().data(), GetRootNodeIDs().size());

		for (const auto& node : GetNodes())
		{
			node >> outputArchive;
		}

		for (const auto& mesh : GetMeshes())
		{
			mesh >> outputArchive;
		}

		for (const auto& blendShape : GetBlendShapes())
		{
			blendShape >> outputArchive;
		}

		for (const auto& morph : GetMorphs())
		{
			morph >> outputArchive;
		}

		for (const auto& material : GetMaterials())
		{
			material >> outputArchive;
		}

		for (const auto& texture : GetTextures())
		{
			texture >> outputArchive;
		}

		for (const auto& camera : GetCameras())
		{
			camera >> outputArchive;
		}

		for (const auto& light : GetLights())
		{
			light >> outputArchive;
		}

		for (const auto& skin : GetSkins())
		{
			skin >> outputArchive;
		}

		for (const auto& skeleton : GetSkeletons())
		{
			skeleton >> outputArchive;
		}

		for (const auto& bone : GetBones())
		{
			bone >> outputArchive;
		}

		for (const auto& animation : GetAnimations())
		{
			animation >> outputArchive;
		}

		for (const auto& track : GetTracks())
		{
			track >> outputArchive;
		}

		for (const auto& particleEmitter : GetParticleEmitters())
		{
			particleEmitter >> outputArchive;
		}

		return *this;
	}
};

}