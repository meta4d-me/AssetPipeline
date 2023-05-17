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
		 const char* filepath = "C:\\TestAssets\\PBR\\te\\DamagedHelmet\\testu\\MR.jpg";
		 const char* filepath2 = "C:\\TestAssets\\PBR\\te\\DamagedHelmet\\testu\\AO.jpg";
		 int width, height,channels;
		 int width2, height2, channels2;
		 auto  texture1 = stbi_load(filepath, &width, &height, &channels, 3);
		 auto  texture2 = stbi_load(filepath2, &width2, &height2, &channels2, 3);
		 if (filepath == NULL)
		 {
			 static_assert("ERROR_FILE_NOT_LOAD");
			 return -1;
		 }
		 if (width != width2)
		 {
			 static_assert("ERROR_SIZE_DIFFERENT");
			 return -1;
		 }
		 if (height != height2)
		 {
			 static_assert("ERROR_SIZE_DIFFERENT");
			 return -1;
		 }


		 std::stringstream ss;
		 ss << pOutputFilePath << "/" << "Split_texture"  << ".png";
		 SplitTextureConsumer consumer1(ss.str().c_str()); 
	     consumer1.init(width, height,texture1);
		 consumer1.merge_RGB(true, true,true, texture1, texture2);
		 Processor processor(nullptr, &consumer1, pSceneDatabase.get());
		 processor.Run();
	}

	return 0;
}