#pragma once

#include "Math/VectorDerived.hpp"

#include <cstdint>
#include <vector>

namespace cdtools
{

struct HeightOctave
{
	HeightOctave(int64_t _seed, float _frequency, float _weight) :
		seed(_seed),
		frequency(_frequency),
		weight(_weight)
	{}

	int64_t seed;
	float frequency;
	float weight;
};

struct TerrainGenParams
{
	uint16_t numSectorsInX;							// number of sectors in x-axis
	uint16_t numSectorsInZ;							// number of sectors in z-axis
	uint16_t numQuadsInSectorInX;					// number of quads in x-axis for each sector
	uint16_t numQuadsInSectorInZ;					// number of quads in z-axis for each sector
	uint16_t quadLengthInX;							// the length of each quad in x-axis
	uint16_t quadLengthInZ;							// the length of each quad in z-axis
	int32_t minElevation;							// the minimum elevation (can be negative) of this entire terrain
	int32_t maxElevation;							// the maximum elevation of this entire terrain
	std::vector<HeightOctave> octaves;	// Set of weights and seeds at powers of 2 frequencies
};

struct TerrainQuad
{
	uint32_t leftTriPolygonId;
	uint32_t rightTriPolygonId;
	uint32_t bottomLeftVertexId;
	uint32_t topLeftVertexId;
	uint32_t topRightVertexId;
	uint32_t bottomRightVertexId;
	cd::Direction bottomLeftNormal;
	cd::Direction topLeftNormal;
	cd::Direction topRightNormal;
	cd::Direction bottomRightNormal;
};

}