#pragma once

#include <stdint.h>

namespace cdtools
{

	class Noise {
	public:
		static float SimplexNoise2D(int64_t seed, double x, double y);

	private:
		Noise() = default;
		~Noise() = default;
		Noise(const Noise&) = delete;
		Noise& operator=(const Noise&) = delete;
		Noise(Noise&&) = delete;
		Noise& operator=(Noise&&) = delete;

	};

}