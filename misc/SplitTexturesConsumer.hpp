#include "Scene/SceneDatabase.h"

#define __STDC_LIB_EXT1__ // prefer sprintf_s
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include"Producers/FbxProducer/FbxProducer.h"

#include <string>
#include <vector>

namespace cdtools
{
class SplitTextureConsumer : public cdtools::IConsumer
{
public:
	SplitTextureConsumer() = delete;
	explicit SplitTextureConsumer(const char* pFilePath) : m_filePath(pFilePath) {}
	SplitTextureConsumer(const SplitTextureConsumer&) = delete;
	SplitTextureConsumer& operator=(const SplitTextureConsumer&) = delete;
	SplitTextureConsumer(SplitTextureConsumer&&) = default;
	SplitTextureConsumer& operator=(SplitTextureConsumer&&) = default;
	virtual ~SplitTextureConsumer() {}

	void init(int width, int height, unsigned char* data, char * type_A)
	{
		m_textureHeight = height;
		m_textureWidth = width;
		texture_data = data;
		type = type_A;
	}


	void live_RGB(bool R_state_s, bool G_state_s, bool B_state_s)
	{
		R_state = R_state_s;
		G_state = G_state_s;
		B_state = B_state_s;
		live = true;
	}

	void merge_RGB(bool R_state_s, bool G_state_s, bool B_state_s, unsigned char* data1, unsigned char* data2)
	{
		R_state = R_state_s;
		G_state = G_state_s;
		B_state = B_state_s;
		texture_data = data1;
		texture_data2 = data2;
		merge_to_p1 = true;
	}

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override
	{
		if (live)
		{
			for (int i = 0; i < m_textureHeight; i++)
			{
				for (int j = 0; j < m_textureWidth; j++)
				{
					if (!R_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)] = 0;
					if (!G_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 1] = 0;
					if (!B_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 2] = 0;
				}
			}
		}
		live = false;


		if (merge_to_p1)
		{
			for (int i = 0; i < m_textureHeight; i++)
			{
				for (int j = 0; j < m_textureWidth; j++)
				{
					if (R_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)];
					if (G_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 1] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 1];
					if (B_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 2] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 2];
				}
			}
		}
		merge_to_p1 = false;

		if (type == "jpg")
		{
			stbi_write_jpg(m_filePath, m_textureWidth, m_textureHeight, m_chanelCount, texture_data, m_textureWidth * m_chanelCount);
		}
		else if (type == "png")
		{
			stbi_write_png(m_filePath, m_textureWidth, m_textureHeight, m_chanelCount, texture_data, m_textureWidth * m_chanelCount);
		}
		else
		{
			static_assert("NULL");
		}
	}


private:
	const char* m_filePath;
	int m_textureWidth;
	int m_textureHeight;
	int m_chanelCount = 3;
	unsigned char* texture_data;
	unsigned char* texture_data2;
	bool R_state;
	bool G_state;
	bool B_state;
	bool live = false;
	bool merge_to_p1 = false;
	char * type = nullptr;

};
}