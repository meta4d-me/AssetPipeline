#include "FbxConsumer.h"
#include "FbxProducer.h"
#include "Framework/Processor.h"
#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "HalfEdgeMesh/Vertex.h"
#include "Scene/SceneDatabase.h"
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
	
	// Import.
	{
		FbxProducer producer(pInputFilePath);
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}
	
	// Processing.
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		auto halfEdgeMesh = cd::hem::HalfEdgeMesh::FromIndexedMesh(mesh);
		assert(halfEdgeMesh.Validate());
	
		halfEdgeMesh.Dump();

		// Filp edges
		{
			for (auto it = halfEdgeMesh.GetEdges().begin(); it != halfEdgeMesh.GetEdges().end(); ++it)
			{
				if (!it->IsOnBoundary())
				{
					cd::Direction result = it->GetHalfEdge()->GetFace()->Normal().Cross(it->GetHalfEdge()->GetTwin()->GetFace()->Normal());
					if (cd::Direction::Zero() == result)
					{
						halfEdgeMesh.SplitEdge(it);
						break;
					}
				}
			}
			halfEdgeMesh.Dump();
		}

		auto convertStrategy = cd::ConvertStrategy::TopologyFirst;
		auto newMesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, convertStrategy);
		newMesh.SetID(cd::MeshID(1U));
		newMesh.SetName("FlipEdgeMesh");
		pSceneDatabase->AddMesh(cd::MoveTemp(newMesh));
	}

	// Export.
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(true);
		processor.Run();
	}

	return 0;
}