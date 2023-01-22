#include "CDConsumer.h"
#include "Framework/Processor.h"
#include "Scene/TerrainTypes.h"
#include "TerrainProducer.h"
#include "Utilities/PerformanceProfiler.h"

#include <random>
#include <vector>

using namespace cd;

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

	TerrainMetadata terrainMetadata;
	terrainMetadata.numSectorsInX = 8;
	terrainMetadata.numSectorsInZ = 8;
	terrainMetadata.minElevation = 0;
	terrainMetadata.maxElevation = 2000;
	terrainMetadata.redistPow = 5.0f;
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 1.0f, 0.0f);
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 2.0f, 0.8f);
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 4.0f, 0.8f);
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 8.0f, 0.5f);
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 16.0f, 0.4f);
	terrainMetadata.octaves.emplace_back(uniform_long(generator), 32.0f, 0.6f);

	TerrainSectorMetadata sectorMetadata;
	sectorMetadata.numQuadsInX = 8;
	sectorMetadata.numQuadsInZ = 8;
	sectorMetadata.quadLenInX = 10;
	sectorMetadata.quadLenInZ = 10;

	TerrainProducer producer(terrainMetadata, sectorMetadata);
	CDConsumer consumer(pOutputFilePath);
	consumer.SetExportMode(cdtools::ExportMode::PureBinary);
	// FbxConsumer consumer(pOutputFilePath);
	Processor processor(&producer, &consumer);
	processor.Run();
	return 0;
}