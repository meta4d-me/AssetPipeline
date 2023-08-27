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
	

	// Import
	{
		GenericProducer producer(pInputFilePath);
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}
	
	// Processing
	//for (const auto& mesh : pSceneDatabase->GetMeshes())
	//{
	//	auto halfEdgeMesh = cd::hem::HalfEdgeMesh::FromIndexedMesh(mesh);
	//	assert(halfEdgeMesh.IsValid());
	//
	//	//halfEdgeMesh.FlipEdge();
	//
	//	auto convertStrategy = cd::ConvertStrategy::TopologyFirst;
	//	auto newMesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, convertStrategy);
	//
	//	if (cd::ConvertStrategy::TopologyFirst == convertStrategy)
	//	{
	//		assert(newMesh.GetVertexCount() == mesh.GetVertexCount());
	//		assert(newMesh.GetPolygonCount() == mesh.GetPolygonCount());
	//	}
	//}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}