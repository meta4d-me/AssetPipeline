#include "Framework/IConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include "UVMapConsumer.hpp"

#include <filesystem>

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

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();
	GenericProducer producer(pInputFilePath);
	producer.ActivateTriangulateService();
	Processor processor(&producer, nullptr, pSceneDatabase.get());
	processor.Run();

	{
		 const char* filepath = "C:\\Toolchain_Scene_Data\\CDSDK_Example\\Models\\textures\\gas_bottle_clean_baseColor.png";
		 int width, height,channels;
		 auto  texture1 =  stbi_load(filepath,&width, &height ,&channels,3);
		 std::stringstream ss;
		 ss << pOutputFilePath << "/" << "Split texture"  << ".png";
		 SplitTextureConsumer consumer1(ss.str().c_str()); \
	     consumer1.init(width, height);
		 consumer1.live_R(texture1);
		 Processor processor(nullptr, &consumer1, pSceneDatabase.get());
		 processor.Run();
	}

	return 0;
}