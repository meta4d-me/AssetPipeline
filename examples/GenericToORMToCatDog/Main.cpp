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
	TextureTypeToColorIndex[cd::MaterialTextureType::Occlusion] = ColorIndex::R;
	TextureTypeToColorIndex[cd::MaterialTextureType::Roughness] = ColorIndex::G;
	TextureTypeToColorIndex[cd::MaterialTextureType::Metallic] = ColorIndex::B;

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

	// Rename material texture file name by type.
	{
		auto RenameMaterialTextureFilePath = [](cd::Material& material, cd::MaterialTextureType textureType, cd::SceneDatabase* pSceneDatabase)
		{
			if (!material.IsTextureSetup(textureType))
			{
				return;
			}

			std::optional<cd::TextureID> optTextureID = material.GetTextureID(textureType);
			if (!optTextureID.has_value())
			{
				return;
			}

			auto& texture = pSceneDatabase->GetTexture(optTextureID.value().Data());

			int width, height;
			free(stbi_load(texture.GetPath(), &width, &height, nullptr, 0));
			std::filesystem::path currentFilePath = texture.GetPath();
			std::string newFilePath = std::format("{}_{}x{}_{}{}", material.GetName(), width, height, cd::GetMaterialPropertyGroupName(textureType), currentFilePath.extension());
			std::filesystem::rename(currentFilePath, newFilePath);
			texture.SetPath(newFilePath.c_str());
		};
	}

	// Set all material's texture path as merged file path.
	{
		for (auto& material : pSceneDatabase->GetMaterials())
		{
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