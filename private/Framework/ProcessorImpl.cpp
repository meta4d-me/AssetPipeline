#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>
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
	for (uint32_t childIndex = 0U; childIndex < node.GetChildIDCount(); ++childIndex)
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

		if (m_pProcessImpl->IsOptionEnabled(cdtools::ProcessorOptions::Validate))
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
		if (m_pProcessImpl->IsOptionEnabled(cdtools::ProcessorOptions::Validate))
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

	// Default Processor Options
	m_options.Enable(ProcessorOptions::Validate);
	m_options.Enable(ProcessorOptions::Dump);
	m_options.Enable(ProcessorOptions::CalculateAABB);

	m_targetAxisSystem = cd::AxisSystem::CDEngine();
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

		if (m_options.IsEnabled(ProcessorOptions::ConvertAxisSystem))
		{
			ConvertAxisSystem();
		}

		if (m_options.IsEnabled(ProcessorOptions::FlattenHierarchy))
		{
			FlattenSceneDatabase();
		}

		if (m_options.IsEnabled(ProcessorOptions::CalculateAABB))
		{
			CalculateAABBForSceneDatabase();
		}

		if (IsSearchMissingTexturesEnabled())
		{
			SearchMissingTextures();
		}

		if (m_options.IsEnabled(ProcessorOptions::EmbedTextureFiles))
		{
			EmbedTextureFiles();
		}
	}

	// Dump all information finally.
	if (m_options.IsEnabled(ProcessorOptions::Dump))
	{
		m_pCurrentSceneDatabase->Dump();
	}

	if (m_pConsumer)
	{
		m_pConsumer->Execute(m_pCurrentSceneDatabase);
	}
}

void ProcessorImpl::ConvertAxisSystem()
{
	const cd::AxisSystem& sceneAxisSystem = m_pCurrentSceneDatabase->GetAxisSystem();
	if (sceneAxisSystem == m_targetAxisSystem)
	{
		return;
	}
	
	bool mirrorHandedness = sceneAxisSystem.GetHandedness() != m_targetAxisSystem.GetHandedness();
	if (mirrorHandedness)
	{
		// TODO : convert node transform data.
		// TODO : convert bone transform data.

		// Convert mesh data.
		for (cd::Mesh& mesh : m_pCurrentSceneDatabase->GetMeshes())
		{
			for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
			{
				auto& position = mesh.GetVertexPosition(vertexIndex);
				position.z() = -position.z();

				for (uint32_t uvSetIndex = 0U; uvSetIndex < mesh.GetVertexUVSetCount(); ++uvSetIndex)
				{
					auto& uv = mesh.GetVertexUV(uvSetIndex, vertexIndex);
					uv.y() = 1.0f - uv.y();
				}

				auto& normal = mesh.GetVertexNormal(vertexIndex);
				normal.z() = -normal.z();
				
				auto& tangent = mesh.GetVertexTangent(vertexIndex);
				tangent.z() = -tangent.z();
				
				auto& bitangent = mesh.GetVertexBiTangent(vertexIndex);
				bitangent.z() = -bitangent.z();
			}

			for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < mesh.GetPolygonGroupCount(); ++polygonGroupIndex)
			{
				auto& polygonGroup = mesh.GetPolygonGroup(polygonGroupIndex);

				for (uint32_t polygonIndex = 0U; polygonIndex < polygonGroup.size(); ++polygonIndex)
				{
					auto& polygon = polygonGroup[polygonIndex];
					uint32_t polygonVertexCount = static_cast<uint32_t>(polygon.size());
					uint32_t polygonVertexHalfCount = polygonVertexCount >> 1;
					for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexHalfCount; ++polygonVertexIndex)
					{
						std::swap(polygon[polygonVertexIndex], polygon[polygonVertexCount - 1 - polygonVertexIndex]);
					}
				}
			}
		}

		// Convert morph data.
		for (cd::Morph& morph : m_pCurrentSceneDatabase->GetMorphs())
		{
			for (uint32_t vertexIndex = 0U; vertexIndex < morph.GetVertexPositionCount(); ++vertexIndex)
			{
				auto& position = morph.GetVertexPosition(vertexIndex);
				position.z() = -position.z();
			}
		}
	}

	m_pCurrentSceneDatabase->SetAxisSystem(m_targetAxisSystem);
}

void ProcessorImpl::CalculateAABBForSceneDatabase()
{
	// Update mesh AABB by its current vertex positions.
	for (auto& mesh : m_pCurrentSceneDatabase->GetMeshes())
	{
		mesh.UpdateAABB();
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
		for (uint32_t nodeMeshIndex = 0U; nodeMeshIndex < node.GetMeshIDCount(); ++nodeMeshIndex)
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
		if (mesh.GetSkinIDCount() > 0U)
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
		if (texture.GetRawData().empty())
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