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
		producer.EnableOption(GenericProducerOptions::FlattenTransformHierarchy);

		MergeTextureConsumer consumer;
		consumer.SetMergedTextureSuffixAndExtension(ormTextureSuffixAndExtension.c_str());
		for (const auto& [textureType, colorIndex] : TextureTypeToColorIndex)
		{
			consumer.SetTextureTypeAndColorIndex(textureType, colorIndex);
		}
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Occlusion, 255);
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Roughness, 128);
		consumer.SetTextureTypeAndDefaultValue(cd::MaterialTextureType::Metallic, 128);

		Processor processor(&producer, &consumer, pSceneDatabase.get());
		processor.DisableOption(ProcessorOptions::Dump);
		processor.Run();
	}

	//auto RenameMaterialTextureFilePath = [](cd::Material& material, cd::MaterialTextureType textureType, cd::SceneDatabase* pSceneDatabase)
	//{
	//	if (!material.IsTextureSetup(textureType))
	//	{
	//		return;
	//	}
	//
	//	std::optional<cd::TextureID> optTextureID = material.GetTextureID(textureType);
	//	if (!optTextureID.has_value())
	//	{
	//		return;
	//	}
	//
	//	auto& texture = pSceneDatabase->GetTexture(optTextureID.value().Data());
	//	std::filesystem::path currentFilePath = texture.GetPath();
	//	if (!std::filesystem::exists(currentFilePath))
	//	{
	//		printf("[RenameMaterialTextureFilePath] : Can't find texture file %s.\n", currentFilePath.string().c_str());
	//		return;
	//	}
	//
	//	int width, height, channel;
	//	stbi_image_free(stbi_load(texture.GetPath(), &width, &height, &channel, 3));
	//
	//	std::string newFilePath = std::format("{}/{}_{}x{}_{}{}", currentFilePath.parent_path().string(),
	//		material.GetName(), width, height, cd::GetMaterialPropertyGroupName(textureType), currentFilePath.extension().string());
	//	std::filesystem::rename(currentFilePath, newFilePath);
	//	texture.SetPath(newFilePath.c_str());
	//};

	// Rename material texture file name by type.
	//{
	//	for (auto& material : pSceneDatabase->GetMaterials())
	//	{
	//		for (int typeIndex = 0; typeIndex < static_cast<int>(cd::MaterialTextureType::Count); ++typeIndex)
	//		{
	//			cd::MaterialTextureType textureType = static_cast<cd::MaterialTextureType>(typeIndex);
	//			RenameMaterialTextureFilePath(material, textureType, pSceneDatabase.get());
	//		}
	//	}
	//}

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