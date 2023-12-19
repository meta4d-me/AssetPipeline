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

			for (const auto& polygonGroup : mesh.GetPolygonGroups())
			{
				for (const auto& polygon : polygonGroup)
				{
					const auto& t0UV = mesh.GetVertexUV(m_uvSetIndex, polygon[0].Data()) - uvOffsetInPicture;
					const auto& t1UV = mesh.GetVertexUV(m_uvSetIndex, polygon[1].Data()) - uvOffsetInPicture;
					const auto& t2UV = mesh.GetVertexUV(m_uvSetIndex, polygon[2].Data()) - uvOffsetInPicture;

					DrawTriangle(CastUVToPoint2d(t0UV), CastUVToPoint2d(t1UV), CastUVToPoint2d(t2UV));
				}
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