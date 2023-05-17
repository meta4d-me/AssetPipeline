#include "Scene/SceneDatabase.h"

#define __STDC_LIB_EXT1__ // prefer sprintf_s
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

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

	void init(int width, int height, unsigned char* data)
	{
		m_textureHeight = height;
		m_textureWidth = width;
		texture_data = data;
	}

	void live_RGB(bool R_state_s, bool G_state_s, bool B_state_s)
	{
		R_state = R_state_s;
		G_state = G_state_s;
		B_state = B_state_s;
		live = true;
	}

	void merge_RGB(bool R_state_s, bool G_state_s, bool B_state_s , unsigned char * data1,unsigned char* data2)
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
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)] =0;
					if (!G_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 1] =0;
					if (!B_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 2] =0;
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
					if(R_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)];
					if(G_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 1] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)+1];
					if(B_state)
						texture_data[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount) + 2] += texture_data2[(i * m_chanelCount * m_textureWidth) + (j * m_chanelCount)+2];
				}
			}
		}
		merge_to_p1 = false;


		stbi_write_jpg("C:\\TestAssets\\PBR\\te\\DamagedHelmet\\testu\\MR.jpg", m_textureWidth, m_textureHeight, m_chanelCount, texture_data, m_textureWidth*m_chanelCount);
	}


private:
	const char* m_filePath;
	int m_textureWidth;
	int m_textureHeight;
	int m_chanelCount= 3;
	unsigned char* texture_data;
	unsigned char* texture_data2;
	bool R_state;
	bool G_state;
	bool B_state;
	bool live = false;
	bool merge_to_p1 = false;

};


class UVMapConsumer : public cdtools::IConsumer
{
public:
	using Point2d = cd::TVector<int, 2>;

public:
	UVMapConsumer() = delete;
	explicit UVMapConsumer(const char* pFilePath) : m_filePath(pFilePath) {}
	UVMapConsumer(const UVMapConsumer&) = delete;
	UVMapConsumer& operator=(const UVMapConsumer&) = delete;
	UVMapConsumer(UVMapConsumer&&) = default;
	UVMapConsumer& operator=(UVMapConsumer&&) = default;
	virtual ~UVMapConsumer() {}

	void SetUVSetIndex(int setIndex)
	{
		m_uvSetIndex = setIndex;
	}

	void SetUVMapUnitSize(int width, int height)
	{
		m_uvmapWidth = width;
		m_uvmapHeight = height;
	}

	void SetUVMapMaxSize(int width, int height)
	{
		m_uvmapMaxWidth = width;
		m_uvmapMaxHeight = height;
	}

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override
	{
		cd::Rect uvmapRect = cd::Rect::Empty();
		for (const auto& mesh : pSceneDatabase->GetMeshes())
		{
			if (mesh.GetVertexUVSetCount() < m_uvSetIndex + 1)
			{
				continue;
			}

			cd::UV minUV(FLT_MAX);
			cd::UV maxUV(FLT_MIN);
			for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
			{
				const auto& uv = mesh.GetVertexUV(m_uvSetIndex, vertexIndex);
				if(uv.x() < minUV.x())
				{
					minUV.x() = uv.x();
				}

				if(uv.x() > maxUV.x())
				{
					maxUV.x() = uv.x();
				}

				if (uv.y() < minUV.y())
				{
					minUV.y() = uv.y();
				}

				if (uv.y() > maxUV.y())
				{
					maxUV.y() = uv.y();
				}
			}

			cd::Rect uvShell(minUV, maxUV);
			if(!uvShell.IsEmpty())
			{
				uvmapRect.Merge(uvShell);
			}
		}

		cd::Vec2f rectSize = uvmapRect.Size();
		int targetUVMapWidth = static_cast<int>(std::ceil(rectSize.x()) * m_uvmapUnitWidth);
		int targetUVMapHeight = static_cast<int>(std::ceil(rectSize.y()) * m_uvmapUnitHeight);

		if(targetUVMapWidth > m_uvmapMaxWidth ||
			targetUVMapHeight > m_uvmapHeight)
		{
			m_uvmapWidth = m_uvmapMaxWidth;
			m_uvmapHeight = m_uvmapMaxHeight;
			int uvmapUnitWidth = static_cast<int>(m_uvmapWidth / std::ceil(rectSize.x()));
			int uvmapUnitHeight = static_cast<int>(m_uvmapHeight / std::ceil(rectSize.y()));

			m_uvmapUnitWidth = std::min(uvmapUnitWidth, uvmapUnitHeight);
			m_uvmapUnitHeight = m_uvmapUnitWidth;
		}
		else
		{
			m_uvmapWidth = targetUVMapWidth;
			m_uvmapHeight = targetUVMapHeight;
		}

		m_uvmapStride = m_uvmapWidth * m_channelCount;
		m_uvmapContent.resize(m_uvmapHeight * m_uvmapStride);
		std::fill(m_uvmapContent.begin(), m_uvmapContent.end(), 0);

		cd::UV uvOffsetInPicture = uvmapRect.Min();
		for (const auto& mesh : pSceneDatabase->GetMeshes())
		{
			if (mesh.GetVertexUVSetCount() < m_uvSetIndex + 1)
			{
				continue;
			}

			for (uint32_t polygonIndex = 0U; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
			{
				const auto& polygon = mesh.GetPolygon(polygonIndex);
				const auto& t0UV = mesh.GetVertexUV(m_uvSetIndex, polygon[0].Data()) - uvOffsetInPicture;
				const auto& t1UV = mesh.GetVertexUV(m_uvSetIndex, polygon[1].Data()) - uvOffsetInPicture;
				const auto& t2UV = mesh.GetVertexUV(m_uvSetIndex, polygon[2].Data()) - uvOffsetInPicture;
				
				DrawTriangle(CastUVToPoint2d(t0UV), CastUVToPoint2d(t1UV), CastUVToPoint2d(t2UV));
			}
		}

		SavePicture();
	}

private:
	Point2d CastUVToPoint2d(const cd::UV& uv)
	{
		float xMax = std::ceil(uv.x());
		float xMin = xMax - 1.0f;
		float yMax = std::ceil(uv.y());
		float yMin = yMax - 1.0f;

		int uvmapX = static_cast<int>(cd::Math::GetValueInNewRange(uv.x(), xMin, xMax, xMin * m_uvmapUnitWidth, xMax * m_uvmapUnitWidth - 1.0f));
		int uvmapY = static_cast<int>(cd::Math::GetValueInNewRange(uv.y(), yMin, yMax, yMin * m_uvmapUnitHeight, yMax * m_uvmapUnitHeight - 1.0f));
		return Point2d(uvmapX, uvmapY);
	}

	void DrawPoint(const Point2d& p)
	{
		int x = std::max(0, p.x());
		int y = std::max(0, p.y());

		m_uvmapContent[(y * m_uvmapWidth + x) * m_channelCount] = m_penColor.x();
		m_uvmapContent[(y * m_uvmapWidth + x) * m_channelCount + 1] = m_penColor.y();
		m_uvmapContent[(y * m_uvmapWidth + x) * m_channelCount + 2] = m_penColor.z();
	}

	void DrawLine(const Point2d& start, const Point2d& end)
	{
		int x1 = start.x();
		int y1 = start.y();
		int x2 = end.x();
		int y2 = end.y();

		float k = 1.0f * (y2 - y1) / (x2 - x1);

		int flag = 0;
		if (k > 1 || k < -1)
		{
			flag = 1;
			x1 ^= y1 ^= x1 ^= y1;
			x2 ^= y2 ^= x2 ^= y2;
			k = 1.0f * (y2 - y1) / (x2 - x1);
		}

		float d = 0.5f - k;
		if (x1 > x2)
		{
			x1 ^= x2 ^= x1 ^= x2;
			y1 ^= y2 ^= y1 ^= y2;
		}

		while (x1 != x2)
		{
			if (k > 0.0f && d < 0.0f)
			{
				++y1;
				++d;
			}
			else if (k < 0.0f && d > 0.0f)
			{
				--y1;
				--d;
			}

			d -= k;
			++x1;

			DrawPoint(flag == 0 ? Point2d(x1, y1) : Point2d(y1, x1));
		}
	}

	void DrawTriangle(const Point2d& t0, const Point2d& t1, const Point2d& t2)
	{
		DrawLine(t0, t1);
		DrawLine(t0, t2);
		DrawLine(t1, t2);
	}

	void SavePicture() const
	{
		stbi_write_png(m_filePath.c_str(), m_uvmapWidth, m_uvmapHeight, m_channelCount, m_uvmapContent.data(), m_uvmapStride);
	}

private:
	uint32_t m_uvSetIndex = 0U;
	int32_t m_channelCount = 3;
	int32_t m_uvmapUnitWidth = 512;
	int32_t m_uvmapUnitHeight = 512;
	int32_t m_uvmapMaxWidth = 4096;
	int32_t m_uvmapMaxHeight = 4096;

	int32_t m_uvmapWidth;
	int32_t m_uvmapHeight;
	int32_t m_uvmapStride;

	std::string m_filePath;
	cd::TVector<uint8_t, 3> m_penColor { 255, 0, 0 };
	std::vector<uint8_t> m_uvmapContent;
};

}