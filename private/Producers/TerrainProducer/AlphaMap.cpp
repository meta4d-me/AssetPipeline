#include "AlphaMap.h"

#include "Math/Math.hpp"
#include "Utilities/Utils.h"

namespace cdtools
{

AlphaMap::AlphaMap() 
	: m_mapType(AlphaMapType::Count)
	, m_textureNames()
	, m_alphaMap()
{}

bool AlphaMap::IsValid() const
{
	return m_mapType != AlphaMapType::Count;
}

void AlphaMap::Clear()
{
	m_mapType = AlphaMapType::Count;
	m_alphaMap.clear();
}

std::string_view AlphaMap::GetTextureNameForChannel(AlphaMapChannel channel) const
{
	assert(channel != AlphaMapChannel::Count);
	return m_textureNames[static_cast<uint8_t>(channel)];
}

void AlphaMap::SetTextureNameForChannel(AlphaMapChannel channel, const char* textureName)
{
	assert(channel != AlphaMapChannel::Count);
	m_textureNames[static_cast<uint8_t>(channel)] = textureName;
}

void AlphaMap::ClearTextureNameForChannel(AlphaMapChannel channel)
{
	assert(channel != AlphaMapChannel::Count);
	m_textureNames[static_cast<uint8_t>(channel)].clear();
}

const std::vector<std::byte>& AlphaMap::GetAlphaMap() const
{
	return m_alphaMap;
}

void AlphaMap::CreateFromElevation(
	const std::vector<int32_t>& elevationMap,
	AlphaMapBlendRegion<int32_t> greenBlendRegion,
	AlphaMapBlendRegion<int32_t> blueBlendRegion,
	AlphaMapBlendRegion<int32_t> alphaBlendRegion,
	AlphaMapBlendFunction blendFuncType) 
{
	assert(greenBlendRegion.blendStart <= greenBlendRegion.blendEnd);
	assert(blueBlendRegion.blendStart <= blueBlendRegion.blendEnd);
	assert(alphaBlendRegion.blendStart <= alphaBlendRegion.blendEnd);
	assert(greenBlendRegion.blendEnd <= blueBlendRegion.blendStart);
	assert(blueBlendRegion.blendEnd <= alphaBlendRegion.blendStart);

	m_mapType = AlphaMapType::Elevation;
	m_alphaMap.clear();
	// We will use RGBA8U here
	// 1 byte per channel; 4 channels per pixel
	m_alphaMap.resize(sizeof(std::byte) * 4 * elevationMap.size());
	size_t j = 0;
	for (size_t i = 0; i < elevationMap.size(); ++i)
	{
		const size_t redChannelIndex = j + 0;
		const size_t greenChannelIndex = j + 1;
		const size_t blueChannelIndex = j + 2;
		const size_t alphaChannelIndex = j + 3;
		const int32_t elevation = elevationMap[i];
		j += 4;	// skip 4 bytes each iteration
		
		// Clear old values
		m_alphaMap[redChannelIndex] = static_cast<std::byte>(0x0);
		m_alphaMap[greenChannelIndex] = static_cast<std::byte>(0x0);
		m_alphaMap[blueChannelIndex] = static_cast<std::byte>(0x0);
		m_alphaMap[alphaChannelIndex] = static_cast<std::byte>(0x0);

		// Calculate alpha map
		const float greenBlendRange = static_cast<float>(greenBlendRegion.blendEnd - greenBlendRegion.blendStart);
		const float blueBlendRange = static_cast<float>(blueBlendRegion.blendEnd - blueBlendRegion.blendStart);
		const float alphaBlendRange = static_cast<float>(alphaBlendRegion.blendEnd - alphaBlendRegion.blendStart);
		if (elevation < greenBlendRegion.blendStart)
		{
			// All red
			m_alphaMap[redChannelIndex] = static_cast<std::byte>(0xFF);
		}
		else if (elevation < greenBlendRegion.blendEnd)
		{
			// Blend between red and green channel textures
			const float t = (elevation - greenBlendRegion.blendStart) / greenBlendRange;
			uint8_t redChannelRatio;
			switch (blendFuncType)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				redChannelRatio = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				redChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				redChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				redChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			m_alphaMap[redChannelIndex] = static_cast<std::byte>(redChannelRatio);
			m_alphaMap[greenChannelIndex] = static_cast<std::byte>(0xFF - redChannelRatio);
		}
		else if (elevation < blueBlendRegion.blendStart)
		{
			// All green
			m_alphaMap[greenChannelIndex] = static_cast<std::byte>(0xFF);
		}
		else if (elevation < blueBlendRegion.blendEnd)
		{
			// Blend between green and blue channel textures
			const float t = (elevation - blueBlendRegion.blendStart) / blueBlendRange;
			uint8_t greenChannelRatio;
			switch (blendFuncType)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				greenChannelRatio = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				greenChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				greenChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				greenChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			m_alphaMap[greenChannelIndex] = static_cast<std::byte>(greenChannelRatio);
			m_alphaMap[blueChannelIndex] = static_cast<std::byte>(0xFF - greenChannelRatio);
		}
		else if (elevation < alphaBlendRegion.blendStart)
		{
			// All blue
			m_alphaMap[blueChannelIndex] = static_cast<std::byte>(0xFF);
		}
		else if (elevation < alphaBlendRegion.blendEnd)
		{
			// Blend between blue and alpha channel textures
			const float t = (elevation - alphaBlendRegion.blendStart) / alphaBlendRange;
			uint8_t blueChannelRatio;
			switch (blendFuncType)
			{
			case cdtools::AlphaMapBlendFunction::Step:
				// no blend since we are stepping
				blueChannelRatio = 0xFF;
				break;
			case cdtools::AlphaMapBlendFunction::Linear:
				blueChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::lerp<float>(0x0, 0xFF, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStep:
				blueChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep<float>(0.0f, 255.0f, t)));
				break;
			case cdtools::AlphaMapBlendFunction::SmoothStepHigh:
				blueChannelRatio = static_cast<uint8_t>(std::ceil(cdtools::smoothstep_high<float>(0.0f, 255.0f, t)));
				break;
			default:
				assert(false);
			}
			m_alphaMap[blueChannelIndex] = static_cast<std::byte>(blueChannelRatio);
			m_alphaMap[alphaChannelIndex] = static_cast<std::byte>(0xFF - blueChannelRatio);
		}
		else
		{
			// all alpha
			m_alphaMap[alphaChannelIndex] = static_cast<std::byte>(0xFF);
		}
	}
}

}