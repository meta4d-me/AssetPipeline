#include "CDConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
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
	//producer.ActivateFlattenHierarchyService();
	producer.ActivateBoundingBoxService();
	producer.ActivateTriangulateService();
	producer.ActivateTangentsSpaceService();
	producer.ActivateCleanUnusedService();

	CDConsumer consumer(pOutputFilePath);
	consumer.SetExportMode(ExportMode::PureBinary);

	Processor processor(&producer, &consumer);
	processor.Run();

	return 0;
}