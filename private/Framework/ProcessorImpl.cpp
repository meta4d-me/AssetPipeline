#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>

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
	printf("\tNode count : %d\n", m_pCurrentSceneDatabase->GetNodeCount());
	printf("\tBone count : %d\n", m_pCurrentSceneDatabase->GetBoneCount());
	printf("\tMesh count : %d\n", m_pCurrentSceneDatabase->GetMeshCount());
	printf("\tMaterial count : %d\n", m_pCurrentSceneDatabase->GetMaterialCount());
	printf("\tTexture count : %d\n", m_pCurrentSceneDatabase->GetTextureCount());
	printf("\n");

	for (const auto& node : m_pCurrentSceneDatabase->GetNodes())
	{
		printf("[Node %u] ParentID : %u, Name : %s\n", node.GetID().Data(), node.GetParentID().Data(), node.GetName().c_str());

		for (cd::MeshID meshID : node.GetMeshIDs())
		{
			const auto& mesh = m_pCurrentSceneDatabase->GetMesh(meshID.Data());
			printf("\t[MeshID %u] name : %s, face number : %u, vertex number : %u, materialID : %u\n", meshID.Data(), mesh.GetName(),
				mesh.GetPolygonCount(), mesh.GetVertexCount(), mesh.GetMaterialID().Data());
		}
	}

	std::vector<cd::MaterialTextureType> textureTypes = {
		cd::MaterialTextureType::BaseColor,
		cd::MaterialTextureType::Occlusion,
		cd::MaterialTextureType::Roughness,
		cd::MaterialTextureType::Metallic,
		cd::MaterialTextureType::Emissive,
		cd::MaterialTextureType::Normal,
	};

	for (const auto& material : m_pCurrentSceneDatabase->GetMaterials())
	{
		printf("[MaterialID %u] %s\n", material.GetID().Data(), material.GetName());

		for (const auto& type : textureTypes)
		{
			if (material.IsTextureSetup(type))
			{
				const auto& textureID = material.GetTextureID(type);
				const auto& texture = m_pCurrentSceneDatabase->GetTexture(textureID.value().Data());
				printf("\t[TextureID %u] %s - %s\n", textureID.value().Data(),
					cd::GetMaterialPropertyGroupName(type), texture.GetPath());
			}
			else
			{
				// printf("\t[warnning] Can not find texture id by textureKey %s\n", textureKey.c_str());
			}
		}
	}

	for (auto& bone : m_pCurrentSceneDatabase->GetBones())
	{
		printf("[Bone %u] ParentID : %u, Name : %s\n", bone.GetID().Data(), bone.GetParentID().Data(), bone.GetName().c_str());
	}
}

void ProcessorImpl::ValidateSceneDatabase()
{
	// TODO : For hierarchy data structs, they are hard to maintain index is same to its ID now.
	// So it needs to keep id and index same. Or consider to use another solution in the future.
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
}

void ProcessorImpl::Run()
{
	m_pProducer->Execute(m_pCurrentSceneDatabase);

	if (IsDumpSceneDatabaseEnabled())
	{
		printf("\nSceneDatabase : %s\n", m_pCurrentSceneDatabase->GetName());
		DumpSceneDatabase();
	}

	if (IsValidateSceneDatabaseEnabled())
	{
		ValidateSceneDatabase();
	}

	m_pConsumer->Execute(m_pCurrentSceneDatabase);
}

}