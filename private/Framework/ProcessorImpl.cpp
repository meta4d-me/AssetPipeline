#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>
#include <cfloat>

namespace details
{

void Dump(const char* label, cd::Quaternion quaternion)
{
	printf("%s : (w = %f, x = %f, y = %f, z = %f)\n", label, quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
}

void Dump(const char* label, cd::Vec3f vector)
{
	printf("%s : (x = %f, y = %f, z = %f)\n", label, vector.x(), vector.y(), vector.z());
}

void Dump(const char* label, cd::Vec4f vector)
{
	printf("%s : (x = %f, y = %f, z = %f, w = %f)\n", label, vector.x(), vector.y(), vector.z(), vector.w());
}

void Dump(const cd::Transform& transform)
{
	details::Dump("\tTranslation", transform.GetTranslation());
	details::Dump("\tRotation", transform.GetRotation());
	details::Dump("\tScale", transform.GetScale());
}

void CalculateNodeTransforms(std::vector<cd::Matrix4x4>& nodeFinalTransforms, const cd::SceneDatabase* pSceneDatabase, const cd::Node& node)
{
	// DFS gurantees that parent node's transform will be calculated before child nodes.
	cd::Matrix4x4 parentTransform = cd::Matrix4x4::Identity();
	if (node.GetParentID().IsValid())
	{
		parentTransform = nodeFinalTransforms[node.GetParentID().Data()];
	}
	cd::Matrix4x4 localTransform = node.GetTransform().GetMatrix();
	nodeFinalTransforms[node.GetID().Data()] = localTransform * parentTransform;

	const std::vector<cd::NodeID>& childIDs = node.GetChildIDs();
	for (uint32_t childIndex = 0U; childIndex < node.GetChildCount(); ++childIndex)
	{
		cd::NodeID childNodeID = childIDs[childIndex];
		CalculateNodeTransforms(nodeFinalTransforms, pSceneDatabase, pSceneDatabase->GetNode(childNodeID.Data()));
	}
};

}

namespace cdtools
{

ProcessorImpl::ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase) :
	m_pProducer(pProducer),
	m_pConsumer(pConsumer)
{
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

void ProcessorImpl::Run()
{
	if (m_pProducer)
	{
		m_pProducer->Execute(m_pCurrentSceneDatabase);
	}

	// Take care to keep correct order for post-process steps.
	if (IsValidateSceneDatabaseEnabled())
	{
		// Pre-validation before doing post-process.
		ValidateSceneDatabase();
	}

	if (IsFlattenSceneDatabaseEnabled())
	{
		FlattenSceneDatabase();
	}

	if (IsCalculateAABBForSceneDatabaseEnabled())
	{
		CalculateAABBForSceneDatabase();
	}

	if (IsValidateSceneDatabaseEnabled())
	{
		// Post-validation after doing post-process.
		ValidateSceneDatabase();
	}

	if (IsCalculateConnetivityDataEnabled())
	{
		CalculateConnetivityData();
	}

	if (IsDumpSceneDatabaseEnabled())
	{
		DumpSceneDatabase();
	}

	if (m_pConsumer)
	{
		m_pConsumer->Execute(m_pCurrentSceneDatabase);
	}
}

void ProcessorImpl::DumpSceneDatabase()
{
	printf("\nSceneDatabase : \n");
	printf("\tName : %s\n", m_pCurrentSceneDatabase->GetName());
	printf("\tAABB :\n");
	details::Dump("\t\tMin", m_pCurrentSceneDatabase->GetAABB().Min());
	details::Dump("\t\tMax", m_pCurrentSceneDatabase->GetAABB().Max());
	printf("\tAxisSystem : \n");
	printf("\t\tHandedness : %s\n", cd::Handedness::Left == m_pCurrentSceneDatabase->GetAxisSystem().GetHandedness() ? "LeftHandSystem" : "RightHandSystem");
	details::Dump("\t\tUpAxis", cd::Vec3f::GetUpAxis(m_pCurrentSceneDatabase->GetAxisSystem()));
	details::Dump("\t\tFrontAxis", cd::Vec3f::GetFrontAxis(m_pCurrentSceneDatabase->GetAxisSystem()));
	printf("\tUnitSystem : \n");
	printf("\t\tUnit : %s\n", cd::Unit::CenterMeter == m_pCurrentSceneDatabase->GetUnit() ? "CenterMeter" : "Meter");

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
			printf("[Node %u] ParentID = %u, Name = %s\n", node.GetID().Data(), node.GetParentID().Data(), node.GetName());
			details::Dump(node.GetTransform());

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
			printf("[Mesh %u] Name = %s\n", mesh.GetID().Data(), mesh.GetName());
			printf("\tVertexCount = %u, TriangleCount = %u\n", mesh.GetVertexCount(), mesh.GetPolygonCount());
			if (mesh.GetMaterialID().IsValid())
			{
				printf("\t[Associated Material %u]\n", mesh.GetMaterialID().Data());

				materialDrawMeshIDs[mesh.GetMaterialID().Data()].push_back(mesh.GetID().Data());
			}

			if constexpr (true)
			{
				for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
				{
					printf("\tVertex [%u]\n", vertexIndex);
					printf("\t\tAdjacent Vertex : ");
					for (cd::VertexID adjVertexID : mesh.GetVertexAdjacentVertexArray(vertexIndex))
					{
						printf("%u, ", adjVertexID.Data());
					}
					printf("\n");

					printf("\t\tAdjacent Polygon : ");
					for (cd::PolygonID adjPolygonID : mesh.GetVertexAdjacentPolygonArray(vertexIndex))
					{
						printf("%u, ", adjPolygonID.Data());
					}
					printf("\n");
				}
			}
		}
	}

	if (m_pCurrentSceneDatabase->GetMaterialCount() > 0U)
	{
		printf("\n");
		for (const auto& material : m_pCurrentSceneDatabase->GetMaterials())
		{
			printf("[MaterialID %u] Name = %s\n", material.GetID().Data(), material.GetName());

			for (int textureTypeValue = 0; textureTypeValue < static_cast<int>(cd::MaterialTextureType::Count); ++textureTypeValue)
			{
				cd::MaterialTextureType textureType = static_cast<cd::MaterialTextureType>(textureTypeValue);
				if (material.IsTextureSetup(textureType))
				{
					const auto& textureID = material.GetTextureID(textureType);
					const auto& texture = m_pCurrentSceneDatabase->GetTexture(textureID.value().Data());
					printf("\t[Associated Texture %u] Type = %s\n", textureID.value().Data(), cd::GetMaterialPropertyGroupName(textureType));
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
			printf("[Texture %u] Type = %s\n", texture.GetID().Data(), cd::GetMaterialPropertyGroupName(texture.GetType()));
			printf("\tPath = %s\n", texture.GetPath());
		}
	}

	if (m_pCurrentSceneDatabase->GetCameraCount() > 0U)
	{
		printf("\n");
		for (const auto& camera : m_pCurrentSceneDatabase->GetCameras())
		{
			printf("[Camera %u] Name = %s\n", camera.GetID().Data(), camera.GetName());
			details::Dump("\tEye", camera.GetEye());
			details::Dump("\tLookAt", camera.GetLookAt());
			details::Dump("\tUp", camera.GetUp());
			printf("\tNearPlane = %f, FarPlane = %f\n", camera.GetNearPlane(), camera.GetFarPlane());
			printf("\tAspect = %f, Fov = %f\n", camera.GetAspect(), camera.GetFov());
		}
	}

	if (m_pCurrentSceneDatabase->GetLightCount() > 0U)
	{
		printf("\n");
		for (const auto& light : m_pCurrentSceneDatabase->GetLights())
		{
			printf("[Light %u] Type = %s, Name = %s\n", light.GetID().Data(), cd::GetLightTypeName(light.GetType()), light.GetName());
			printf("\tIntensity = %f\n", light.GetIntensity());
			printf("\tRange = %f, Radius = %f\n", light.GetRange(), light.GetRadius());
			printf("\tWidth = %f, Height = %f\n", light.GetWidth(), light.GetHeight());
			printf("\tAngleScale = %f, AngleOffset = %f\n", light.GetAngleScale(), light.GetAngleOffset());
			details::Dump("\tColor", light.GetColor());
			details::Dump("\tPosition", light.GetPosition());
			details::Dump("\tDirection", light.GetDirection());
			details::Dump("\tUp", light.GetUp());
		}
	}

	if (m_pCurrentSceneDatabase->GetBoneCount() > 0U)
	{
		printf("\n");
		for (const auto& bone : m_pCurrentSceneDatabase->GetBones())
		{
			printf("[Bone %u] Name : %s, ParentID : %u\n", bone.GetID().Data(), bone.GetName(), bone.GetParentID().Data());
			details::Dump(bone.GetTransform());

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

			details::Dump("\tFirstTranslationKey", track.GetTranslationKeys()[0].GetValue());
			details::Dump("\tFirstRotationKey", track.GetRotationKeys()[0].GetValue());
			details::Dump("\tFirstScaleKey", track.GetScaleKeys()[0].GetValue());
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
		float keyFrameTime = -FLT_MAX;
		for (const auto& key : track.GetTranslationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MAX;
		for (const auto& key : track.GetRotationKeys())
		{
			float keyTime = key.GetTime();
			assert(keyFrameTime < keyTime);
			keyFrameTime = keyTime;
		}

		keyFrameTime = -FLT_MAX;
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

		//assert(m_pCurrentSceneDatabase->GetBoneByName(track.GetName()));
		CheckKeyFramesTimeOrder(track);
	}
}

void ProcessorImpl::CalculateConnetivityData()
{
	for (auto& mesh : m_pCurrentSceneDatabase->GetMeshes())
	{
		uint32_t vertexCount = mesh.GetVertexCount();
		mesh.GetVertexAdjacentVertexArrays().resize(vertexCount);
		mesh.GetVertexAdjacentPolygonArrays().resize(vertexCount);

		uint32_t polygonCount = mesh.GetPolygonCount();
		for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
		{
			const auto& polygon = mesh.GetPolygon(polygonIndex);
			static_assert(3U == polygon.Size);
			
			mesh.AddVertexAdjacentVertexID(polygon[0].Data(), polygon[1]);
			mesh.AddVertexAdjacentVertexID(polygon[0].Data(), polygon[2]);
			mesh.AddVertexAdjacentVertexID(polygon[1].Data(), polygon[0]);
			mesh.AddVertexAdjacentVertexID(polygon[1].Data(), polygon[2]);
			mesh.AddVertexAdjacentVertexID(polygon[2].Data(), polygon[0]);
			mesh.AddVertexAdjacentVertexID(polygon[2].Data(), polygon[1]);

			mesh.AddVertexAdjacentPolygonID(polygon[0].Data(), cd::PolygonID(polygonIndex));
			mesh.AddVertexAdjacentPolygonID(polygon[1].Data(), cd::PolygonID(polygonIndex));
			mesh.AddVertexAdjacentPolygonID(polygon[2].Data(), cd::PolygonID(polygonIndex));
		}

		for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
		{
			cd::VertexIDArray& adjVertexIDs = mesh.GetVertexAdjacentVertexArray(vertexIndex);
			std::sort(adjVertexIDs.begin(), adjVertexIDs.end(), [](cd::VertexID lhs, cd::VertexID rhs) { return lhs < rhs; });

			cd::PolygonIDArray& adjPolygonIDs = mesh.GetVertexAdjacentPolygonArray(vertexIndex);
			std::sort(adjPolygonIDs.begin(), adjPolygonIDs.end(), [](cd::PolygonID lhs, cd::PolygonID rhs) { return lhs < rhs; });
		}
	}
}

void ProcessorImpl::CalculateAABBForSceneDatabase()
{
	// Update mesh AABB by its current vertex positions.
	std::vector<cd::Mesh>& meshes = m_pCurrentSceneDatabase->GetMeshes();
	for (uint32_t meshIndex = 0U; meshIndex < m_pCurrentSceneDatabase->GetMeshCount(); ++meshIndex)
	{
		cd::Mesh& mesh = meshes[meshIndex];

		// Apply transform to vertex position.
		cd::Point minPoint(FLT_MAX);
		cd::Point maxPoint(-FLT_MAX);
		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
			minPoint.x() = minPoint.x() > position.x() ? position.x() : minPoint.x();
			minPoint.y() = minPoint.y() > position.y() ? position.y() : minPoint.y();
			minPoint.z() = minPoint.z() > position.z() ? position.z() : minPoint.z();
			maxPoint.x() = maxPoint.x() > position.x() ? maxPoint.x() : position.x();
			maxPoint.y() = maxPoint.y() > position.y() ? maxPoint.y() : position.y();
			maxPoint.z() = maxPoint.z() > position.z() ? maxPoint.z() : position.z();
		}

		mesh.SetAABB(cd::AABB(cd::MoveTemp(minPoint), cd::MoveTemp(maxPoint)));
	}

	// Update scene AABB by meshes' AABB.
	m_pCurrentSceneDatabase->UpdateAABB();
}

void ProcessorImpl::FlattenSceneDatabase()
{
	uint32_t totalNodeCount = m_pCurrentSceneDatabase->GetNodeCount();
	if (0U == totalNodeCount)
	{
		return;
	}

	// Init every node's final transform matrix after removing node hierarchy.
	std::vector<cd::Matrix4x4> nodeFinalTransforms;
	nodeFinalTransforms.reserve(totalNodeCount);
	std::map<uint32_t, uint32_t> mapMeshIDToAssociatedNodeID;
	for (uint32_t nodeIndex = 0U; nodeIndex < totalNodeCount; ++nodeIndex)
	{
		const cd::Node& node = m_pCurrentSceneDatabase->GetNode(nodeIndex);
		const std::vector<cd::MeshID>& nodeMeshIDs = node.GetMeshIDs();
		for (uint32_t nodeMeshIndex = 0U; nodeMeshIndex < node.GetMeshCount(); ++nodeMeshIndex)
		{
			mapMeshIDToAssociatedNodeID[nodeMeshIDs[nodeMeshIndex].Data()] = nodeIndex;
		}

		nodeFinalTransforms.push_back(cd::Transform::Identity().GetMatrix());
	}

	const cd::Node& rootNode = m_pCurrentSceneDatabase->GetNode(0);
	details::CalculateNodeTransforms(nodeFinalTransforms, m_pCurrentSceneDatabase, rootNode);

	std::vector<cd::Mesh>& meshes = m_pCurrentSceneDatabase->GetMeshes();
	for (uint32_t meshIndex = 0U; meshIndex < m_pCurrentSceneDatabase->GetMeshCount(); ++meshIndex)
	{
		cd::Mesh& mesh = meshes[meshIndex];
		if (0U != mesh.GetVertexInfluenceCount())
		{
			// Don't need to support flatten SkinMesh currently.
			continue;
		}

		// Apply transform to vertex position.
		auto itNodeIndex = mapMeshIDToAssociatedNodeID.find(meshIndex);
		if (itNodeIndex == mapMeshIDToAssociatedNodeID.end())
		{
			// If a mesh doesn't find its associated node, no need to process.
			continue;
		}

		uint32_t nodeIndex = itNodeIndex->second;
		const cd::Matrix4x4& finalTransform = nodeFinalTransforms[nodeIndex];
		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
			cd::Vec4f newPosition = finalTransform * cd::Vec4f(position.x(), position.y(), position.z(), 1.0f);
			mesh.SetVertexPosition(vertexIndex, cd::Point(newPosition.x(), newPosition.y(), newPosition.z()));
		}
	}

	// Delete all nodes.
	m_pCurrentSceneDatabase->GetNodes().clear();
	m_pCurrentSceneDatabase->SetNodeCount(0U);
}

}