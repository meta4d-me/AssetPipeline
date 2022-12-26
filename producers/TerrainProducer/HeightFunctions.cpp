#include "HeightFunctions.h"

#include "Noise/Noise.h"

#include <math.h>
#include <random>

namespace
{

std::default_random_engine generator;
std::uniform_int_distribution<int32_t> uniform_int = std::move(std::uniform_int_distribution<int32_t>(INT_MIN, INT_MAX));
std::uniform_int_distribution<int64_t> uniform_long = std::move(std::uniform_int_distribution<int64_t>(LONG_MIN, LONG_MAX));

float GetNoiseAt(double nx, double nz)
{
	return cdtools::Noise::SimplexNoise2D(uniform_long(generator), nx, nz);
}

float GetNoiseAt(int64_t seed, double nx, double nz)
{
	return cdtools::Noise::SimplexNoise2D(seed, nx, nz);
}

// Distance functions, all nx and nz should be in the range of [-1, 1] which is normalized across map length
float SquareBump(double nx, double nz)
{
	return static_cast<float>(1 - (1 - nx * nx) * (1 - nz * nz));
}

float EuclideanSquared(double nx, double nz)
{
	return std::min(1.0f, static_cast<float>((nx * nx + nz * nz) / sqrt(2)));
}

float Diagonal(double nx, double nz)
{
	return static_cast<float>(std::max(abs(nx), abs(nz)));
}

float Manhattan(double nx, double nz)
{
	return static_cast<float>((abs(nx) + abs(nz)) / 2.0f);
}

float Euclidean(double nx, double nz)
{
	return static_cast<float>((nx * nx + nz * nz) / sqrt(2));
}

float Hyperboloid(double nx, double nz)
{
	const float offset = 0.2f;
	return static_cast<float>((nx * nx + nz * nz + offset * offset) / ((2 + offset * offset) - offset));
}

}

namespace cdtools
{

//static
float HeightFunctions::GetDefaultHeight(
	const uint32_t x,
	const uint32_t z,
	const uint32_t terrainLenInX,
	const uint32_t terrainLenInZ,
	const float maxHeight,
	const float redistPower,
	const std::vector<HeightOctave>& octaves)
{
	const double nx = x / static_cast<double>(terrainLenInX);
	const double nz = z / static_cast<double>(terrainLenInZ);
	float height = 0.0f;
	float totalWeight = 0.0f;
	for (uint32_t i = 0; i < octaves.size(); ++i)
	{
		const HeightOctave& octave = octaves[i];
		height += octave.weight * GetNoiseAt(octave.seed, octave.frequency * nx, octave.frequency * nz);
		totalWeight += octave.weight;
	}
	height /= totalWeight;
	height = pow(height, redistPower);
	height *= maxHeight;
	return height;
}

}
