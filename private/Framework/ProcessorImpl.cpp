#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>
#include <cfloat>

namespace cdtools
{

ProcessorImpl::ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase) :
	m_pProducer(pProducer),
	m_pConsumer(pConsumer)
{
	assert(pProducer && "pProducer is invalid.");
	assert(pConsumer && "pConsumer is invalid.");

	if (nullptr == pHostSceneDatabase)
	{
		// Processor will create a SceneDatabase managed by itself.
		m_pLocalSceneDatabase = std::make_unique<cd::SceneDatabase>();
		m_pCurrentSceneDatabase = m_pLocalSceneDatabase.get();
	}
	else
	{
		// The SceneDatabase is from outside resource.
		m_pCurrentSceneDatabase = pHostSceneDatabase;
	}
}

ProcessorImpl::~ProcessorImpl()
{
}

void ProcessorImpl::DumpSceneDatabase()
{
	printf("\nSceneDatabase : %s\n", m_pCurrentSceneDatabase->GetName());
	printf("\tNode count : %d\n", m_pCurrentSceneDatabase->GetNodeCount());
	printf("\tMesh count : %d\n", m_pCurrentSceneDatabase->GetMeshCount());
	printf("\tMaterial count : %d\n", m_pCurrentSceneDatabase->GetMaterialCount());
	printf("\tTexture count : %d\n", m_pCurrentSceneDatabase->GetTextureCount());
	printf("\tCamera count : %d\n", m_pCurrentSceneDatabase->GetCameraCount());
	printf("\tLight count : %d\n", m_pCurrentSceneDatabase->GetLightCount());
	printf("\tBone count : %d\n", m_pCurrentSceneDatabase->GetBoneCount());
	printf("\tAnimation count : %d\n", m_pCurrentSceneDatabase->GetAnimationCount());
	printf("\tTrack count : %d\n", m_pCurrentSceneDatabase->GetTrackCount());

	if (m_pCurrentSceneDatabase->GetNodeCount() > 0U)
	{
		printf("\n");
		for (const auto& node : m_pCurrentSceneDatabase->GetNodes())
		{
			printf("[Node %u] ParentID : %u, Name : %s\n", node.GetID().Data(), node.GetParentID().Data(), node.GetName());
			const auto& nodeTransform = node.GetTransform();
			const auto& translation = nodeTransform.GetTranslation();
			printf("\tTranslation : (x = %f, y = %f, z = %f)\n", translation.x(), translation.y(), translation.z());
			const auto& rotation = nodeTransform.GetRotation();
			printf("\tRotation    : (w = %f, x = %f, y = %f, z = %f)\n", rotation.w(), rotation.x(), rotation.y(), rotation.z());
			const auto& scale = nodeTransform.GetScale();
			printf("\tScale       : (x = %f, y = %f, z = %f)\n", scale.x(), scale.y(), scale.z());

			for (cd::MeshID meshID : node.GetMeshIDs())
			{
				printf("\t[Associated Mesh %u]\n", meshID.Data());
			}
		}
	}

	std::map<uint32_t, std::vector<uint32_t>> materialDrawMeshIDs;
	if (m_pCurrentSceneDatabase->GetMeshCount() > 0U)
	{
		printf("\n");
		for (const auto& mesh : m_pCurrentSceneDatabase->GetMeshes())
		{
			printf("[Mesh %u] Name : %s\n", mesh.GetID().Data(), mesh.GetName());
			printf("\tVertexCount : %u, TriangleCount : %u\n", mesh.GetVertexCount(), mesh.GetPolygonCount());
			if (mesh.GetMaterialID().IsValid())
			{
				printf("\t[Associated Material %u]\n", mesh.GetMaterialID().Data());

				materialDrawMeshIDs[mesh.GetMaterialID().Data()].push_back(mesh.GetID().Data());
			}
		}
	}

	if (m_pCurrentSceneDatabase->GetMaterialCount() > 0U)
	{
		printf("\n");
		for (const auto& material : m_pCurrentSceneDatabase->GetMaterials())
		{
			printf("[MaterialID %u] %s\n", material.GetID().Data(), material.GetName());

			for (int textureTypeValue = 0; textureTypeValue < static_cast<int>(cd::MaterialTextureType::Count); ++textureTypeValue)
			{
				cd::MaterialTextureType textureType = static_cast<cd::MaterialTextureType>(textureTypeValue);
				if (material.IsTextureSetup(textureType))
				{
					const auto& textureID = material.GetTextureID(textureType);
					const auto& texture = m_pCurrentSceneDatabase->GetTexture(textureID.value().Data());
					printf("\t[Associated Texture %u] %s\n", textureID.value().Data(), cd::GetMaterialPropertyGroupName(textureType));
					printf("\t\tPath = %s\n", texture.GetPath());
				}
			}

			if (auto itDrawMeshes = materialDrawMeshIDs.find(material.GetID().Data());
				itDrawMeshes != materialDrawMeshIDs.end())
			{
				for (uint32_t drawMeshID : itDrawMeshes->second)
				{
					const auto& mesh = m_pCurrentSceneDatabase->GetMesh(drawMeshID);
					printf("\t[Associated Mesh %u] %s \n", drawMeshID, mesh.GetName());
				}
			}
		}
	}

	if (m_pCurrentSceneDatabase->GetTextureCount() > 0U)
	{
		printf("\n");
		for (const auto& texture : m_pCurrentSceneDatabase->GetTextures())
		{
			printf("[Texture %u] %s\n", texture.GetID().Data(), cd::GetMaterialPropertyGroupName(texture.GetType()));
			printf("\tPath = %s\n", texture.GetPath());
		}
	}

	if (m_pCurrentSceneDatabase->GetCameraCount() > 0U)
	{
		printf("\n");
		for (const auto& camera : m_pCurrentSceneDatabase->GetCameras())
		{
			printf("[Camera %u] %s\n", camera.GetID().Data(), camera.GetName());
		}
	}

	if (m_pCurrentSceneDatabase->GetLightCount() > 0U)
	{
		printf("\n");
		for (const auto& light : m_pCurrentSceneDatabase->GetLights())
		{
			printf("[Light %u]\n", light.GetID().Data());
		}
	}

	if (m_pCurrentSceneDatabase->GetBoneCount() > 0U)
	{
		printf("\n");
		for (const auto& bone : m_pCurrentSceneDatabase->GetBones())
		{
			printf("[Bone %u] Name : %s, ParentID : %u\n", bone.GetID().Data(), bone.GetName(), bone.GetParentID().Data());
			const auto& boneTransform = bone.GetTransform();
			const auto& translation = boneTransform.GetTranslation();
			printf("\tTranslation : (x = %f, y = %f, z = %f)\n", translation.x(), translation.y(), translation.z());
			const auto& rotation = boneTransform.GetRotation();
			printf("\tRotation    : (w = %f, x = %f, y = %f, z = %f)\n", rotation.w(), rotation.x(), rotation.y(), rotation.z());
			const auto& scale = boneTransform.GetScale();
			printf("\tScale       : (x = %f, y = %f, z = %f)\n", scale.x(), scale.y(), scale.z());

			for (const cd::BoneID childNodeID : bone.GetChildIDs())
			{
				printf("\t[ChildBone %u]\n", childNodeID.Data());
			}
		}
	}

	if (m_pCurrentSceneDatabase->GetAnimationCount() > 0U)
	{
		printf("\n");
		for (const auto& animation : m_pCurrentSceneDatabase->GetAnimations())
		{
			printf("[Animation %u] Name : %s\n", animation.GetID().Data(), animation.GetName());
			printf("\tDuration : %f, TicksPerSecond : %f\n", animation.GetDuration(), animation.GetTicksPerSecnod());
		}
	}

	if (m_pCurrentSceneDatabase->GetTrackCount() > 0U)
	{
		printf("\n");
		for (const auto& track : m_pCurrentSceneDatabase->GetTracks())
		{
			printf("[Track %u] Name : %s\n", track.GetID().Data(), track.GetName());
			printf("\tTranslationKeyCount : %u, RotationKeyCount : %u, ScaleKeyCount : %u\n",
				track.GetTranslationKeyCount(), track.GetRotationKeyCount(), track.GetScaleKeyCount());

			const auto& translationKeyValue = track.GetTranslationKeys()[0].GetValue();
			printf("\tFirstTranslationKey : (x = %f, y = %f, z = %f)\n", translationKeyValue.x(), translationKeyValue.y(), translationKeyValue.z());
			const auto& rotationKeyValue = track.GetRotationKeys()[0].GetValue();
			printf("\tFirstRotationKey    : (w = %f, x = %f, y = %f, z = %f)\n", rotationKeyValue.w(), rotationKeyValue.x(), rotationKeyValue.y(), rotationKeyValue.z());
			const auto& scaleKeyValue = track.GetScaleKeys()[0].GetValue();
			printf("\tFirstScaleKey       : (x = %f, y = %f, z = %f)\n", scaleKeyValue.x(), scaleKeyValue.y(), scaleKeyValue.z());
		}
	}
}

void ProcessorImpl::ValidateSceneDatabase()
{
	for (uint32_t nodeIndex = 0U; nodeIndex < m_pCurrentSceneDatabase->GetNodeCount(); ++nodeIndex)
	{
		const cd::Node& node = m_pCurrentSceneDatabase->GetNode(nodeIndex);
		assert(nodeIndex == node.GetID().Data());
	}

	for (uint32_t meshIndex = 0U; meshIndex < m_pCurrentSceneDatabase->GetMeshCount(); ++meshIndex)
	{
		const cd::Mesh& mesh = m_pCurrentSceneDatabase->GetMesh(meshIndex);
		assert(meshIndex == mesh.GetID().Data());
	}

	for (uint32_t materialIndex = 0U; materialIndex < m_pCurrentSceneDatabase->GetMaterialCount(); ++materialIndex)
	{
		const cd::Material& material = m_pCurrentSceneDatabase->GetMaterial(materialIndex);
		assert(materialIndex == material.GetID().Data());
	}

	for (uint32_t textureIndex = 0U; textureIndex < m_pCurrentSceneDatabase->GetTextureCount(); ++textureIndex)
	{
		const cd::Texture& texture = m_pCurrentSceneDatabase->GetTexture(textureIndex);
		assert(textureIndex == texture.GetID().Data());
	}

	for (uint32_t boneIndex = 0U; boneIndex < m_pCurrentSceneDatabase->GetBoneCount(); ++boneIndex)
	{
		const cd::Bone& bone = m_pCurrentSceneDatabase->GetBone(boneIndex);
		assert(boneIndex == bone.GetID().Data());
	}

	for (uint32_t animationIndex = 0U; animationIndex < m_pCurrentSceneDatabase->GetAnimationCount(); ++animationIndex)
	{
		const cd::Animation& animation = m_pCurrentSceneDatabase->GetAnimation(animationIndex);
		assert(animationIndex == animation.GetID().Data());
	}

	auto CheckKeyFramesTimeOrder = [](const cd::Track& track)
	{
		// Make sure keyframes are sorted by time LessNotEqual.
		float keyFrameTime = -FLT_MIN;
		for (const auto& key : track.GetTranslationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MIN;
		for (const auto& key : track.GetRotationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MIN;
		for (const auto& key : track.GetScaleKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}
	};

	for (uint32_t trackIndex = 0U; trackIndex < m_pCurrentSceneDatabase->GetTrackCount(); ++trackIndex)
	{
		const cd::Track& track = m_pCurrentSceneDatabase->GetTrack(trackIndex);
		assert(trackIndex == track.GetID().Data());
		assert(track.GetTranslationKeyCount() > 0 || track.GetRotationKeyCount() > 0 || track.GetScaleKeyCount() > 0);

		assert(m_pCurrentSceneDatabase->GetBoneByName(track.GetName()));
		CheckKeyFramesTimeOrder(track);
	}
}

void ProcessorImpl::CalculateAABBForSceneDatabase()
{
	cd::AABB sceneAABB(0.0f, 0.0f);
	for (const auto& mesh : m_pCurrentSceneDatabase->GetMeshes())
	{
		sceneAABB.Merge(mesh.GetAABB());
	}
	m_pCurrentSceneDatabase->SetAABB(cd::MoveTemp(sceneAABB));
}

void ProcessorImpl::Run()
{
	m_pProducer->Execute(m_pCurrentSceneDatabase);

	if (IsDumpSceneDatabaseEnabled())
	{
		DumpSceneDatabase();
	}

	if (IsValidateSceneDatabaseEnabled())
	{
		ValidateSceneDatabase();
	}

	if (IsCalculateAABBForSceneDatabaseEnabled())
	{
		CalculateAABBForSceneDatabase();
	}

	m_pConsumer->Execute(m_pCurrentSceneDatabase);
}

}