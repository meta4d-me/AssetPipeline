#include "CDConsumer.h"
#include "Framework/Processor.h"
#include "EffekseerProducer.h"
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

	PerformanceProfiler profiler("AssetPipeline");

	// TODO : how to input char16_t string from argv?
	const char16_t* pInputFilePath = u"S:/Effekseer/Examples/Resources/Laser01.efkefc";
	const char* pOutputFilePath = argv[2];
	EffekseerProducer producer(pInputFilePath);
	CDConsumer consumer(pOutputFilePath);
	consumer.SetExportMode(ExportMode::PureBinary);

	Processor processor(&producer, &consumer);
	processor.SetDumpSceneDatabaseEnable(true);
	processor.SetValidateSceneDatabaseEnable(true);
	processor.Run();

	return 0;
}