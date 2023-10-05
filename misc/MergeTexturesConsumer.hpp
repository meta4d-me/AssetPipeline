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
	G,
	B
};

constexpr int RequestChannelCount = 3;

using Texture2DRect = cd::TVector<int, 2>;

struct Texture2D
{
	stbi_uc* data = nullptr;
	int size = 0;
	Texture2DRect rect;
	int channel = 0;

	~Texture2D()
	{
		if (data)
		{
			stbi_image_free(data);
			data = nullptr;
		}
	}

	void Allocate()
	{
		assert(data == nullptr);
		size = rect.x() * rect.y() * channel;
		assert(size > 0);
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

class MergeTextureConsumer : public cdtools::IConsumer
{
public:
	MergeTextureConsumer() = default;
	MergeTextureConsumer(const char* pOutputFolderPath) : m_outputFolderPath(pOutputFolderPath) {}
	MergeTextureConsumer(const MergeTextureConsumer&) = delete;
	MergeTextureConsumer& operator=(const MergeTextureConsumer&) = delete;
	MergeTextureConsumer(MergeTextureConsumer&&) = default;
	MergeTextureConsumer& operator=(MergeTextureConsumer&&) = default;
	virtual ~MergeTextureConsumer() = default;

	void SetMergedTextureSuffixAndExtension(const char* pSuffixAndExtension)
	{
		m_mergedTextureSuffixAndExtension = pSuffixAndExtension;
	}

	void SetTextureTypeAndDefaultValue(cd::MaterialTextureType textureType, uint8_t value)
	{
		m_textureColorDefaultValue[textureType] = value;
	}

	void SetTextureTypeAndColorIndex(cd::MaterialTextureType textureType, ColorIndex colorIndex)
	{
		m_textureColorIndex[textureType] = colorIndex;
	}

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override
	{
		assert(!m_mergedTextureSuffixAndExtension.empty() && "Need to specify merged texture suffix and output file extension.");
		assert(!m_textureColorIndex.empty() && "Forgot to set texture type and its according color index?");

		std::map<cd::TextureID, std::string> textureNewFilePath;
		for (const auto& material : pSceneDatabase->GetMaterials())
		{
			std::map<std::string, std::vector<cd::MaterialTextureType>> textureFileSupportTypes;
			for (const auto& [textureType, _] : m_textureColorIndex)
			{
				if (!material.IsTextureSetup(textureType))
				{
					continue;
				}

				cd::TextureID textureID = material.GetTextureID(textureType);
				const auto& texture = pSceneDatabase->GetTexture(textureID.Data());
				textureFileSupportTypes[texture.GetPath()].push_back(textureType);
			}

			if (textureFileSupportTypes.size() < 1)
			{
				// Material only has one texture type.
				// TextureTypes already share same texture file path.
				continue;
			}

			// Validate all texture files have same width and height.
			// Validate all texture files can be loaded successfully.
			// TODO : texture type is a valid 2D texture.
			// TextureType : 2D, 3D, Volume, Cubemap...
			std::vector<Texture2D> texture2DStorage;
			texture2DStorage.reserve(textureFileSupportTypes.size());

			std::map<cd::MaterialTextureType, Texture2D*> loadedTexturesData;
			std::optional<Texture2DRect> mergedTextureRect;
			std::filesystem::path mergedTextureFilePath;
			bool isAnyUnknownTexture = false;
			bool isSameTextureSize = true;
			for (const auto& [textureFilePath, textureTypes] : textureFileSupportTypes)
			{
				Texture2D& texture2D = texture2DStorage.emplace_back();
				texture2D.channel = RequestChannelCount;
				texture2D.data = stbi_load(textureFilePath.c_str(), &texture2D.rect.x(), &texture2D.rect.y(), nullptr, RequestChannelCount);
				texture2D.size = texture2D.rect.x() * texture2D.rect.y() * texture2D.channel;
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
					loadedTexturesData[textureType] = &texture2D;
				}
			}

			if (isAnyUnknownTexture || !isSameTextureSize)
			{
				continue;
			}

			// Only need to consider 8bit per pixel channel because stb will help convert 16 to 8.
			Texture2D mergedTexture;
			mergedTexture.rect = mergedTextureRect.value();
			mergedTexture.channel = RequestChannelCount;
			mergedTexture.Allocate();

			for (const auto& [textureType, colorValue] : m_textureColorDefaultValue)
			{
				// Init default color value if file not loaded.
				if (loadedTexturesData.find(textureType) == loadedTexturesData.end())
				{
					assert(m_textureColorDefaultValue.find(textureType) != m_textureColorDefaultValue.end());
					int colorIndex = static_cast<int>(m_textureColorIndex[textureType]);
					uint8_t defaultValue = m_textureColorDefaultValue[textureType];
					for (int i = 0; i < mergedTexture.rect.x(); ++i)
					{
						for (int j = 0; j < mergedTexture.rect.y(); ++j)
						{
							mergedTexture.SetPixelData(i, j, colorIndex, defaultValue);
						}
					}
				}
			}

			for (const auto& [textureType, pTexture2D] : loadedTexturesData)
			{
				int colorIndex = static_cast<int>(m_textureColorIndex[textureType]);
				for (int i = 0; i < mergedTexture.rect.x(); ++i)
				{
					for (int j = 0; j < mergedTexture.rect.y(); ++j)
					{
						mergedTexture.SetPixelData(i, j, colorIndex, pTexture2D->GetPixelData(i, j, colorIndex));
					}
				}
			}

			std::filesystem::path mergedFilePath = !m_outputFolderPath.empty() ? m_outputFolderPath : mergedTextureFilePath.parent_path();
			mergedFilePath /= mergedTextureFilePath.stem();
			//mergedFilePath += "_" + std::to_string(mergedTextureRect.value().x());
			//mergedFilePath += "x" + std::to_string(mergedTextureRect.value().y());
			mergedFilePath += "_" + m_mergedTextureSuffixAndExtension;
			std::string outputFilePath = mergedFilePath.string();
			if (!mergedTexture.Save(outputFilePath.c_str()))
			{
				printf("Failed to save %s while merging textures.\n", outputFilePath.c_str());
			}
			else
			{
				printf("Succeed to save merged texture : %s.\n", outputFilePath.c_str());
				for (const auto& [textureType, _] : m_textureColorIndex)
				{
					if (!material.IsTextureSetup(textureType))
					{
						continue;
					}

					cd::TextureID textureID = material.GetTextureID(textureType);
					textureNewFilePath[textureID] = outputFilePath;
				}
			}
		}

		// Update material texture paths.
		for (const auto& [textureID, filePath] : textureNewFilePath)
		{
			auto& texture = const_cast<cd::Texture&>(pSceneDatabase->GetTexture(textureID.Data()));
			texture.SetPath(filePath.c_str());
		}
	}

private:
	std::string m_outputFolderPath;
	std::string m_mergedTextureSuffixAndExtension;
	std::map<cd::MaterialTextureType, ColorIndex> m_textureColorIndex; 
	std::map<cd::MaterialTextureType, uint8_t> m_textureColorDefaultValue; 
};
}