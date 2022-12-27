#pragma once

#include "TerrainParameter.h"

namespace cdtools
{

class HeightFunctions
{
public:
	static float GetDefaultHeight(
		const uint32_t x,
		const uint32_t z, 
		const uint32_t terrainLenInX, 
		const uint32_t terrainLenInZ,
		const float maxHeight,
		const float redistPower,
		const std::vector<HeightOctave>& octaves);
};

}
