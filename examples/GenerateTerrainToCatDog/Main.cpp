#include "Consumer/CatDogConsumer.h"
// #include "Consumer/FbxConsumer.h"
#include "Processor/Processor.h"
#include "Producer/TerrainProducer.h"
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

	using namespace cdtools;

	PerformanceProfiler profiler("TerrainGenerator");

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];
	TerrainProducer producer(256, 256, 500, 500, 6000);
	CatDogConsumer consumer(pOutputFilePath);
	// FbxConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();
	return 0;
}