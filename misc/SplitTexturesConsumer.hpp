#include "Scene/SceneDatabase.h"

#define __STDC_LIB_EXT1__ // prefer sprintf_s
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <filesystem>
#include <string>
#include <vector>

namespace cdtools
{

enum class ColorIndex
{
	R = 0,
	G = 1,
	B = 2
};

constexpr int RequestChannelCount = 3;

using Texture2DRect = cd::TVector<int, 2>;

struct Texture2D
{
	stbi_uc* data = nullptr;
	int size = 0;
	Texture2DRect rect;
	int channel = 0;

	void Allocate()
	{
		assert(data == nullptr);
		size = rect.x() * rect.y() * channel;
		assert(size != 0);
		data = new stbi_uc[size];
		memset(data, 0, size);
	}

	stbi_uc GetPixelData(int i, int j, int colorIndex) const
	{
		return data[(i * channel * rect.x()) + (j * channel) + colorIndex];
	}

	void SetPixelData(int i, int j, int colorIndex, stbi_uc value)
	{
		data[(i * channel * rect.x()) + (j * channel) + colorIndex] = value;
	}

	bool Save(const char* pFilePath)
	{
		return 1 == stbi_write_png(pFilePath, rect.x(), rect.y(), channel, data, rect.x() * channel);
	}
};

class SplitTextureConsumer : public cdtools::IConsumer
{
public:
	SplitTextureConsumer() = default;
	SplitTextureConsumer(const SplitTextureConsumer&) = delete;
	SplitTextureConsumer& operator=(const SplitTextureConsumer&) = delete;
	SplitTextureConsumer(SplitTextureConsumer&&) = default;
	SplitTextureConsumer& operator=(SplitTextureConsumer&&) = default;
	virtual ~SplitTextureConsumer() {}

	void SetTextureTypeAndColorIndex(cd::MaterialTextureType textureType, ColorIndex colorIndex)
	{
		m_textureColorIndex[textureType] = colorIndex;
	}

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override
	{
		assert(!m_textureColorIndex.empty() && "Forgot to set texture type and its according color index?");

		for (const auto& material : pSceneDatabase->GetMaterials())
		{
			std::map<std::string, std::vector<cd::MaterialTextureType>> textureFileSupportTypes;
			for (const auto& [textureType, _] : m_textureColorIndex)
			{
				if (!material.IsTextureSetup(textureType))
				{
					continue;
				}

				cd::TextureID textureID = material.GetTextureID(textureType).value();
				const auto& texture = pSceneDatabase->GetTexture(textureID.Data());
				textureFileSupportTypes[texture.GetPath()].push_back(textureType);
			}

			if (textureFileSupportTypes.size() <= 1)
			{
				// Material only has one texture type.
				// TextureTypes already share same texture file path.
				continue;
			}

			// Validate all texture files have same width and height.
			// Validate all texture files can be loaded successfully.
			// TODO : texture type is a valid 2D texture.
			// TextureType : 2D, 3D, Volume, Cubemap...
			std::map<cd::MaterialTextureType, Texture2D> loadedTexturesData;
			std::optional<Texture2DRect> mergedTextureRect;
			std::filesystem::path mergedTextureFilePath;
			bool isAnyUnknownTexture = false;
			bool isSameTextureSize = true;
			for (const auto& [textureFilePath, textureTypes] : textureFileSupportTypes)
			{
				Texture2D texture2D;
				texture2D.data = stbi_load(textureFilePath.c_str(), &texture2D.rect.x(), &texture2D.rect.y(), &texture2D.channel, RequestChannelCount);
				if (!texture2D.data)
				{
					// Failed to load texture file.
					isAnyUnknownTexture = true;
					break;
				}

				if (!mergedTextureRect.has_value())
				{
					mergedTextureRect = texture2D.rect;
					mergedTextureFilePath = textureFilePath.c_str();
				}
				else if (mergedTextureRect != texture2D.rect)
				{
					isSameTextureSize = false;
					break;
				}

				for (cd::MaterialTextureType textureType : textureTypes)
				{
					loadedTexturesData[textureType] = texture2D;
				}
			}

			if (isAnyUnknownTexture || !isSameTextureSize)
			{
				// TODO : free heap memory.
				continue;
			}

			// Only need to consider 8bit per pixel channel because stb will help convert 16 to 8.
			Texture2D mergedTexture;
			mergedTexture.rect = mergedTextureRect.value();
			mergedTexture.channel = RequestChannelCount;
			mergedTexture.Allocate();
			for (const auto& [textureType, texture2D] : loadedTexturesData)
			{
				int colorIndex = static_cast<int>(m_textureColorIndex[textureType]);
				for (int i = 0; i < mergedTexture.rect.x(); ++i)
				{
					for (int j = 0; j < mergedTexture.rect.y(); ++j)
					{
						mergedTexture.SetPixelData(i, j, colorIndex, texture2D.GetPixelData(i, j, colorIndex));
					}
				}
			}

			std::filesystem::path mergedFilePath = mergedTextureFilePath.parent_path();
			mergedFilePath = mergedFilePath / material.GetName();
			mergedFilePath += "_orm.png";
			std::string outputFilePath = mergedFilePath.string();
			if (!mergedTexture.Save(outputFilePath.c_str()))
			{
				printf("Failed to save %s while merging textures.", outputFilePath.c_str());
			}
		}
	}

private:
	std::map<cd::MaterialTextureType, ColorIndex> m_textureColorIndex; 
};
}