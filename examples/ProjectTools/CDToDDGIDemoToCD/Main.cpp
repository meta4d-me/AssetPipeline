#include "CDConsumer.h"
#include "Framework/Processor.h"
#include "CDProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include "Scene/SceneDatabase.h"

#include <filesystem>
#include <memory>

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
	CDProducer producer(pInputFilePath);
	CDConsumer consumer(pOutputFilePath);

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();
	{
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(false);
		processor.SetValidateSceneDatabaseEnable(true);
		processor.Run();
	}

	{
		std::vector<const char*> removeMaterialNames = { "Floor_Tiles_03", "WoodFloor" };
		for (auto& name : removeMaterialNames)
		{
			for (auto& material : pSceneDatabase->GetMaterials())
			{
				if (strcmp(material.GetName(), name) == 0)
				{
					material.RemoveTexture(cd::MaterialPropertyGroup::Metallic);
					material.RemoveTexture(cd::MaterialPropertyGroup::Occlusion);
					material.RemoveTexture(cd::MaterialPropertyGroup::Roughness);
				}
			}
		}

		for (auto& texture : pSceneDatabase->GetTextures())
		{
			std::filesystem::path texturePath = texture.GetPath();
			texture.SetPath(texturePath.filename().replace_extension(".dds").string().c_str());
		}

		//for (auto& light : pSceneDatabase->GetLights())
		//{
		//	light.SetIntensity(light.GetIntensity() * 0.8f);
		//}
	}

	{
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(true);
		processor.SetValidateSceneDatabaseEnable(true);
		processor.Run();
	}

	return 0;
}