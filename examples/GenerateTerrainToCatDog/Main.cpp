#include "CDConsumer.h"
#include "Framework/Processor.h"
#include "HeightFunctions.h"
#include "TerrainProducer.h"
#include "Utilities/PerformanceProfiler.h"

#include <random>
#include <vector>

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
	// TODO get these from file
	std::default_random_engine generator;
	std::uniform_int_distribution<int64_t> uniform_long = std::uniform_int_distribution<int64_t>(LONG_MIN, LONG_MAX);

	TerrainGenParams generationParams;
	generationParams.numSectorsInX = 8;
	generationParams.numSectorsInZ = 8;
	generationParams.numQuadsInSectorInX = 32;
	generationParams.numQuadsInSectorInZ = 32;
	generationParams.quadLengthInX = 100;
	generationParams.quadLengthInZ = 100;
	generationParams.minElevation = 0;
	generationParams.maxElevation = 2000;
	generationParams.octaves.emplace_back(uniform_long(generator), 1.0f, 0.0f);
	generationParams.octaves.emplace_back(uniform_long(generator), 2.0f, 0.8f);
	generationParams.octaves.emplace_back(uniform_long(generator), 4.0f, 0.8f);
	generationParams.octaves.emplace_back(uniform_long(generator), 8.0f, 0.5f);
	generationParams.octaves.emplace_back(uniform_long(generator), 16.0f, 0.4f);
	generationParams.octaves.emplace_back(uniform_long(generator), 32.0f, 0.6f);

	TerrainProducer producer(generationParams);
	CDConsumer consumer(pOutputFilePath);
	consumer.SetExportMode(cdtools::ExportMode::PureBinary);
	// FbxConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();
	return 0;
}