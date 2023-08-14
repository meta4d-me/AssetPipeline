#include "FbxConsumer.h"
#include "GenericProducer.h"
#include "Framework/Processor.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
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
	
	// Generate a source mesh.
	{
		GenericProducer producer(pInputFilePath);
		producer.ActivateTriangulateService();
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}
	
	{
		for (const auto& mesh : pSceneDatabase->GetMeshes())
		{
			auto halfEdgeMesh = cd::hem::HalfEdgeMesh::FromIndexedMesh(mesh);
			assert(halfEdgeMesh.Validate());
			auto newMesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::TopologyFirst);
			assert(newMesh.GetVertexCount() > 0U);
		}
	}

	return 0;
}