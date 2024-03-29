#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "ProgressiveMesh/ProgressiveMesh.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if (argc != 3)
	{
		return 1;
	}

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];
	
	using namespace cdtools;
	PerformanceProfiler profiler("ProgressiveMesh");
	
	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	// Import
	{
		GenericProducer producer(pInputFilePath);
		producer.EnableOption(GenericProducerOptions::FlattenTransformHierarchy);
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}
	
	// Processing
	std::map<cd::MeshID, cd::NodeID> mapMeshToNodeId;
	for (uint32_t nodeIndex = 0U; nodeIndex < pSceneDatabase->GetNodeCount(); ++nodeIndex)
	{
		const auto& node = pSceneDatabase->GetNode(nodeIndex);
		for (uint32_t meshIndex = 0U; meshIndex < node.GetMeshIDCount(); ++meshIndex)
		{
			mapMeshToNodeId[node.GetMeshID(meshIndex)] = nodeIndex;
		}
	}

	uint32_t meshCount = pSceneDatabase->GetMeshCount();
	for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
	{
		const auto& mesh = pSceneDatabase->GetMesh(meshIndex);
		uint32_t vertexCount = mesh.GetVertexCount();

		std::vector<uint32_t> indexBuffer;
		for (const auto& polygonGroup : mesh.GetPolygonGroups())
		{
			for (const auto& polygon : polygonGroup)
			{
				assert(polygon.size() == 3U && "Need to triangulate.");

				for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygon.size(); ++polygonVertexIndex)
				{
					indexBuffer.push_back(polygon[polygonVertexIndex].Data());
				}
			}
		}

		auto pm = cd::ProgressiveMesh::FromIndexedMesh(mesh);
		for (uint32_t otherMeshIndex = 0U; otherMeshIndex < meshCount; ++otherMeshIndex)
		{
			if (meshIndex == otherMeshIndex)
			{
				continue;
			}

			const auto& otherMesh = pSceneDatabase->GetMesh(otherMeshIndex);
			if (!mesh.GetAABB().Intersects(otherMesh.GetAABB()))
			{
				continue;
			}

			cd::AABB intersection = mesh.GetAABB().GetIntersection(otherMesh.GetAABB());
			pm.InitBoundary(intersection);
		}

		auto lodMesh = pm.GenerateLodMesh(0.5f, 3000, &mesh);
		lodMesh.SetName(std::format("{}_reduced", mesh.GetName()).c_str());
		cd::MeshID lodMeshID(pSceneDatabase->GetMeshCount());
		lodMesh.SetID(lodMeshID);
		lodMesh.AddMaterialID(cd::MaterialID::InvalidID);

		cd::NodeID nodeID = mapMeshToNodeId[mesh.GetID()];
		pSceneDatabase->GetNode(nodeID.Data()).AddMeshID(lodMeshID);

		pSceneDatabase->AddMesh(cd::MoveTemp(lodMesh));
	}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}