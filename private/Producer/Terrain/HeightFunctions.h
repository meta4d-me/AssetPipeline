#pragma once

#include <vector>
#include <stdint.h>

namespace cdtools
{

class HeightFunctions
{
public:
	struct Octave
	{
		int64_t seed;
		float frequency;
		float weight;
	};

	static float GetDefaultHeight(
		const uint32_t x,
		const uint32_t z, 
		const uint32_t terrainLenInX, 
		const uint32_t terrainLenInZ,
		const float maxHeight,
		const float redistPower,
		const std::vector<Octave>& octaves);
};

}
