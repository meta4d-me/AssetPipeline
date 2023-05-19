#include "Framework/IConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include"SplitTexturesConsumer.hpp"


#include <filesystem>

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : input file2 path 
	if (argc != 3)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("AssetPipeline");

	const char* pInputFilePath = argv[1];
	const char* pInputFilePath2 = argv[2];

	if (pInputFilePath == NULL && pInputFilePath2 == NULL)
	{
		static_assert("INPUT NULL");
		return -1;
	}
	else if (pInputFilePath == NULL && pInputFilePath2 != NULL)
	{
		pInputFilePath = pInputFilePath2;
	}
	else if (pInputFilePath2 == NULL && pInputFilePath != NULL)
	{
		pInputFilePath2 = pInputFilePath;
	}


		char * type = nullptr;
		const char* pDot = strrchr(pInputFilePath, '.');
		const char* pDot2 = strrchr(pInputFilePath2, '.');

		if (strcmp(pDot, pDot2))
		{
			static_assert("TYPE_DIFFERENT");
			return 0;
		}

		if (!strcmp(pDot, ".jpg"))
		{
			type = "jpg";
		}
		else if (!strcmp(pDot, ".png"))
		{
			type = "png";
		}
		else if (!strcmp(pDot, ".fbx"))
		{
			FbxProducer fbx(pInputFilePath);
		}
		else
		{

		}




	     auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();
		 int width, height,channels;
		 int width2, height2, channels2;
		 auto  texture1 = stbi_load(pInputFilePath, &width, &height, &channels, 3);
		 auto  texture2 = stbi_load(pInputFilePath2, &width2, &height2, &channels2, 3);
		 if (width != width2)
		 {
			 static_assert("ERROR_WIDE_DIFFERENT");
			 return -1;
		 }
		 if (height != height2)
		 {
			 static_assert("ERROR_HEIGHT_DIFFERENT");
			 return -1;
		 }


		 SplitTextureConsumer consumer1(pInputFilePath); 
	     consumer1.init(width, height,texture1 , type);
		 consumer1.merge_RGB(true, true,true, texture1, texture2);
		 Processor processor(nullptr, &consumer1, pSceneDatabase.get());
		 processor.Run();
	

		return 0;
}