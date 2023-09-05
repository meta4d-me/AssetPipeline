#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>
#include <cfloat>
#include <filesystem>
#include <fstream>

namespace details
{

void CalculateNodeTransforms(std::vector<cd::Matrix4x4>& nodeFinalTransforms, const cd::SceneDatabase* pSceneDatabase, const cd::Node& node)
{
	// DFS gurantees that parent node's transform will be calculated before child nodes.
	cd::Matrix4x4 parentTransform = cd::Matrix4x4::Identity();
	if (node.GetParentID().IsValid())
	{
		parentTransform = nodeFinalTransforms[node.GetParentID().Data()];
	}

	const cd::Transform& sourceTransform = node.GetTransform();
	cd::Matrix4x4 localTransform = sourceTransform.GetMatrix();
	nodeFinalTransforms[node.GetID().Data()] = parentTransform * localTransform;

	const std::vector<cd::NodeID>& childIDs = node.GetChildIDs();
	for (uint32_t childIndex = 0U; childIndex < node.GetChildCount(); ++childIndex)
	{
		cd::NodeID childNodeID = childIDs[childIndex];
		CalculateNodeTransforms(nodeFinalTransforms, pSceneDatabase, pSceneDatabase->GetNode(childNodeID.Data()));
	}
};

class SceneDatabaseValidator
{
public:
	SceneDatabaseValidator() = delete;
	explicit SceneDatabaseValidator(cdtools::ProcessorImpl* pProcessorImpl)
	{
		assert(pProcessorImpl);
		m_pProcessImpl = pProcessorImpl;

		if (m_pProcessImpl->IsValidateSceneDatabaseEnabled())
		{
			m_pProcessImpl->GetSceneDatabase()->Validate();
		}
	}
	SceneDatabaseValidator(const SceneDatabaseValidator&) = delete;
	SceneDatabaseValidator& operator=(const SceneDatabaseValidator&) = delete;
	SceneDatabaseValidator(SceneDatabaseValidator&&) = delete;
	SceneDatabaseValidator& operator=(SceneDatabaseValidator&&) = delete;
	~SceneDatabaseValidator()
	{
		if (m_pProcessImpl->IsValidateSceneDatabaseEnabled())
		{
			m_pProcessImpl->GetSceneDatabase()->Validate();
		}
	}

private:
	cdtools::ProcessorImpl* m_pProcessImpl = nullptr;
};

std::vector<std::byte> LoadFile(const char* pFilePath)
{
	std::vector<std::byte> fileData;

	std::ifstream fin(pFilePath, std::ios::in | std::ios::binary);
	if (!fin.is_open())
	{
		return fileData;
	}

	fin.seekg(0L, std::ios::end);
	size_t fileSize = fin.tellg();
	fin.seekg(0L, std::ios::beg);
	fileData.resize(fileSize);
	fin.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	fin.close();

	return fileData;
}

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

	// Adding post processing here.
	// SceneDatabaseValidator will help to validate if data is correct before and after.
	{
		details::SceneDatabaseValidator validator(this);

		if (IsFlattenSceneDatabaseEnabled())
		{
			FlattenSceneDatabase();
		}

		if (IsCalculateAABBForSceneDatabaseEnabled())
		{
			CalculateAABBForSceneDatabase();
		}

		if (IsSearchMissingTexturesEnabled())
		{
			SearchMissingTextures();
		}

		if (IsEmbedTextureFilesEnabled())
		{
			EmbedTextureFiles();
		}
	}

	// Dump all information finally.
	if (IsDumpSceneDatabaseEnabled())
	{
		m_pCurrentSceneDatabase->Dump();
	}

	if (m_pConsumer)
	{
		m_pConsumer->Execute(m_pCurrentSceneDatabase);
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

void ProcessorImpl::SearchMissingTextures()
{
	for (auto& texture : m_pCurrentSceneDatabase->GetTextures())
	{
		std::filesystem::path originFilePath(texture.GetPath());
		if (std::filesystem::exists(originFilePath))
		{
			continue;
		}

		for (const std::string& textureSearchFolder : m_textureSearchFolders)
		{
			std::filesystem::path newFilePath(textureSearchFolder);
			newFilePath /= originFilePath.filename();

			if (std::filesystem::exists(newFilePath))
			{
				texture.SetPath(newFilePath.string().c_str());
				break;
			}
		}
	}
}

void ProcessorImpl::EmbedTextureFiles()
{
	for (auto& texture : m_pCurrentSceneDatabase->GetTextures())
	{
		if (texture.ExistRawData())
		{
			continue;
		}

		const char* pFilePath = texture.GetPath();
		if (!std::filesystem::exists(pFilePath))
		{
			continue;
		}

		// Just embed texture file, not parse its information.
		texture.SetRawData(details::LoadFile(pFilePath));
	}
}

}