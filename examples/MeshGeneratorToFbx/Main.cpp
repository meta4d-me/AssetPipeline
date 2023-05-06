#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "Utilities/PerformanceProfiler.h"

#include "MeshGenerteProducer.hpp"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if (argc != 3)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("MeshGenerator");

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];
	MeshGenerteProducer producer("");
	FbxConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();
	return 0;
}