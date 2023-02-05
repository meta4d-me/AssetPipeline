#pragma once

#include "Base/Export.h"

#include <stdint.h>

namespace cd
{

class CORE_API NoiseGenerator final
{
public:
	// Utility class doesn't allow to construct.
	explicit NoiseGenerator() = delete;
	NoiseGenerator(const NoiseGenerator&) = delete;
	NoiseGenerator& operator=(const NoiseGenerator&) = delete;
	NoiseGenerator(NoiseGenerator&&) = delete;
	NoiseGenerator& operator=(NoiseGenerator&&) = delete;
	~NoiseGenerator() = delete;

	/* 
	 * Generate noise a specified coordinate in the range of [-1, 1]. If normalized, it 
	 * returns in the range of [0, 1]
	 */
	static float SimplexNoise2D(int64_t seed, double x, double y, bool normalize = true);

};

}