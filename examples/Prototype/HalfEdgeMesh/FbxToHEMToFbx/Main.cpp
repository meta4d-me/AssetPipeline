#include "FbxConsumer.h"
#include "FbxProducer.h"
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
	auto pNewSceneDatabase = std::make_unique<cd::SceneDatabase>();

	// Import
	{
		FbxProducer producer(pInputFilePath);
		producer.SetWantTriangulate(false);
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}

	// Processing
	for (auto& mesh : pSceneDatabase->GetMeshes())
	{
		mesh.RemoveDuplicatedVertices();
		auto halfEdgeMesh = cd::HalfEdgeMesh::FromIndexedMesh(mesh);
		//assert(halfEdgeMesh.IsValid());

		auto newMesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::BoundaryOnly);
		newMesh.SetName(mesh.GetName());
		newMesh.SetID(pNewSceneDatabase->GetMeshCount());
		pNewSceneDatabase->AddMesh(cd::MoveTemp(newMesh));
	}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pNewSceneDatabase.get());
		processor.Run();
	}

	return 0;
}