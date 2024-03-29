#include "CDConsumer.h"
#include "FbxProducer.h"
#include "Framework/Processor.h"
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
	FbxProducer producer(pInputFilePath);
	producer.EnableOption(FbxProducerOptions::ImportAnimation);
	producer.EnableOption(FbxProducerOptions::ImportBlendShape);
	producer.EnableOption(FbxProducerOptions::ImportMaterial);
	producer.EnableOption(FbxProducerOptions::ImportTexture);
	producer.EnableOption(FbxProducerOptions::ImportSkeleton);
	producer.EnableOption(FbxProducerOptions::ImportSkeletalMesh);
	producer.EnableOption(FbxProducerOptions::ImportLight);
	producer.EnableOption(FbxProducerOptions::Triangulate);
	CDConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();

	return 0;
}