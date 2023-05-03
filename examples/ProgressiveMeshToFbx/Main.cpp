#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "Utilities/PerformanceProfiler.h"

#include "MeshGenerteProducer.hpp"
#include "ProgressiveMeshProducer.hpp"

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

	{
		// Generate a source mesh.
		MeshGenerteProducer producer("");
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.SetCalculateConnetivityDataEnable(true);
		processor.Run();
	}

	{
		// Generate a progressive target mesh.
		ProgressiveMeshProducer producer("");
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(&producer, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}