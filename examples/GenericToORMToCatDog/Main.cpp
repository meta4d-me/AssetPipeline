#include "CDConsumer.h"
#include "Framework/IConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include "MergeTexturesConsumer.hpp"

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

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];

	using namespace cdtools;
	PerformanceProfiler profiler("AssetPipeline");

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	std::map<cd::MaterialTextureType, ColorIndex> TextureTypeToColorIndex;
	TextureTypeToColorIndex[cd::MaterialTextureType::Occlusion] = ColorIndex::R;
	TextureTypeToColorIndex[cd::MaterialTextureType::Roughness] = ColorIndex::G;
	TextureTypeToColorIndex[cd::MaterialTextureType::Metallic] = ColorIndex::B;

	std::map<cd::MaterialTextureType, std::string> TextureTypeToStringType;
	TextureTypeToStringType[cd::MaterialTextureType::BaseColor] = "BaseColor";
	TextureTypeToStringType[cd::MaterialTextureType::Occlusion] = "Occlusion";
	TextureTypeToStringType[cd::MaterialTextureType::Roughness] = "Roughness";
	TextureTypeToStringType[cd::MaterialTextureType::Metallic] = "Metallic";
	TextureTypeToStringType[cd::MaterialTextureType::Normal] = "Normal";
	TextureTypeToStringType[cd::MaterialTextureType::Emissive] = "Emissive";
	TextureTypeToStringType[cd::MaterialTextureType::Elevation] = "Elevation";
	TextureTypeToStringType[cd::MaterialTextureType::AlphaMap] = "AlphaMap";
	TextureTypeToStringType[cd::MaterialTextureType::General] = "General";

	// Import a generic model file and merge its material texture files as standard ORM PBR workflow.
	std::string ormTextureSuffixAndExtension = "orm.png";
	{
		GenericProducer producer(pInputFilePath);
		producer.ActivateBoundingBoxService();
		producer.ActivateTriangulateService();
		producer.ActivateTangentsSpaceService();
		producer.ActivateCleanUnusedService();

		MergeTextureConsumer consumer;
		consumer.SetMergedTextureSuffixAndExtension(ormTextureSuffixAndExtension.c_str());
		for (const auto& [textureType, colorIndex] : TextureTypeToColorIndex)
		{
			consumer.SetTextureTypeAndColorIndex(textureType, colorIndex);
		}
		Processor processor(&producer, &consumer, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(false);
		processor.Run();
	}

	// Set all material's texture path as merged file path.
	{
		for (auto& material : pSceneDatabase->GetMaterials())
		{
			for (const auto& [textureType, _] : TextureTypeToStringType)
			{
				if (!material.IsTextureSetup(textureType))
				{
					continue;
				}

				std::optional<cd::TextureID> optTextureID = material.GetTextureID(textureType);
				auto& texture = pSceneDatabase->GetTexture(optTextureID.value().Data());
				std::filesystem::path mergedTextureFilePath = texture.GetPath();
				mergedTextureFilePath = mergedTextureFilePath.parent_path() / material.GetName();
				mergedTextureFilePath += "_" + std::to_string(texture.GetWidth());
				mergedTextureFilePath += "x" + std::to_string(texture.GetHeight());
				mergedTextureFilePath += "_" + TextureTypeToStringType[textureType];
				mergedTextureFilePath += ".jpg";
				std::filesystem::path current_Name = texture.GetPath();
				texture.SetPath(mergedTextureFilePath.string().c_str());
				std::filesystem::path new_Name = texture.GetPath();
				std::filesystem::rename(current_Name, new_Name);
			}

			for (const auto& [textureType, _] : TextureTypeToColorIndex)
			{
				if (!material.IsTextureSetup(textureType))
				{
					continue;
				}

				std::optional<cd::TextureID> optTextureID = material.GetTextureID(textureType);
				auto& texture = pSceneDatabase->GetTexture(optTextureID.value().Data());
				std::filesystem::path mergedTextureFilePath = texture.GetPath();
				mergedTextureFilePath = mergedTextureFilePath.parent_path() / material.GetName();
				mergedTextureFilePath += "_" + std::to_string(texture.GetWidth());
				mergedTextureFilePath += "x" + std::to_string(texture.GetHeight());
				mergedTextureFilePath += "_" + ormTextureSuffixAndExtension;
				texture.SetPath(mergedTextureFilePath.string().c_str());
			}
		}
	}

	// Save current scene database as a .cdbin file.
	{
		CDConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}