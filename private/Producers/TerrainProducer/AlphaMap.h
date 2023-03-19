#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "TerrainProducer/AlphaMapTypes.h"

#include <array>
#include <stdint.h>
#include <string_view>
#include <vector>

namespace cdtools
{

class AlphaMap {
public:
	explicit AlphaMap();
	AlphaMap(const AlphaMap& rhs) = delete;
	AlphaMap& operator=(const AlphaMap& rhs) = delete;
	AlphaMap(AlphaMap&& rhs) = delete;
	AlphaMap& operator=(AlphaMap&& rhs) = delete;
	~AlphaMap() = default;

	bool IsValid() const;
	void Clear();
	std::string_view GetTextureNameForChannel(AlphaMapChannel channel) const;
	void SetTextureNameForChannel(AlphaMapChannel channel, const char* textureName);
	void ClearTextureNameForChannel(AlphaMapChannel channel);
	const std::vector<std::byte>& GetAlphaMap() const;

	void CreateFromElevation(
		const std::vector<int32_t>& elevationMap,
		AlphaMapBlendRegion<int32_t> greenBlendRegion,
		AlphaMapBlendRegion<int32_t> blueBlendRegion,
		AlphaMapBlendRegion<int32_t> alphaBlendRegion,
		AlphaMapBlendFunction blendFuncType);

private:
	AlphaMapType m_mapType;
	std::array<std::string, 4> m_textureNames;
	std::vector<std::byte> m_alphaMap;
};

}