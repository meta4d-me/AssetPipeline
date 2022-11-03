#include "Consumer/CatDogConsumer.h"
#include "Processor/Processor.h"
#include "Producer/GenericProducer.h"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if(argc != 3)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("AssetPipeline");

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];
	GenericProducer producer(pInputFilePath);
	producer.ActivateFlattenHierarchyService();
	producer.ActivateTriangulateService();
	producer.ActivateTangentsSpaceService();
	CatDogConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();
	return 0;
}