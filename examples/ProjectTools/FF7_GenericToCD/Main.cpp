#include "CDConsumer.h"
#include "Framework/IConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include "MergeTexturesConsumer.hpp"

#include <filesystem>
#include <format>

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if (argc != 3)
	{
		return 1;
	}

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];

	using namespace cdtools;
	PerformanceProfiler profiler("AssetPipeline");

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	std::map<cd::MaterialTextureType, ColorIndex> TextureTypeToColorIndex;
	TextureTypeToColorIndex[cd::MaterialTextureType::Metallic] = ColorIndex::R;
	TextureTypeToColorIndex[cd::MaterialTextureType::Roughness] = ColorIndex::G;

	std::filesystem::path outputTextureFolderPath = pOutputFilePath;
	outputTextureFolderPath = outputTextureFolderPath.parent_path() / "textures";
	if (!std::filesystem::exists(outputTextureFolderPath))
	{
		std::filesystem::create_directories(outputTextureFolderPath);
	}

	// Import a generic model file and merge its material texture files as standard ORM PBR workflow.
	std::string ormTextureSuffixAndExtension = "ORM.png";
	{
		GenericProducer producer(pInputFilePath);
		producer.EnableOption(GenericProducerOptions::FlattenTransformHierarchy);

		MergeTextureConsumer consumer(outputTextureFolderPath.string().c_str());
		consumer.SetMergedTextureSuffixAndExtension(ormTextureSuffixAndExtension.c_str());
		for (const auto& [textureType, colorIndex] : TextureTypeToColorIndex)
		{
			consumer.SetTextureTypeAndColorIndex(textureType, colorIndex);
		}
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Occlusion, 255);
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Roughness, 230);
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Metallic, 25);

		Processor processor(&producer, &consumer, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(false);
		processor.Run();
	}

	// Save current scene database as a .cdbin file.
	{
		CDConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}