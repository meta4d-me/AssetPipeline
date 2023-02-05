#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"

#include <stdint.h>
#include <vector>

namespace cdtools
{

/*
 * Octave data used to feed into Simplex2D noise function to compute the elevation of a
 * Terrain. Seeds are fed into random generator for consistency, frequencies are usually
 * powers of 2 representing different noise levels that when combined create interesting
 * results. In a way, it tunes smoothness where lower (and lack of) frequencies will 
 * produce smoother results rather than more rough outcomes. Finally we can also tune
 * the weights of each frequency and they should be normalized in the end when summing.
 */
struct ElevationOctave
{
	explicit ElevationOctave() = default;

	explicit ElevationOctave(int64_t _seed, float _frequency, float _weight) 
		: seed(_seed)
		, frequency(_frequency)
		, weight(_weight)
	{}

	template<bool SwapBytesOrder>
	explicit ElevationOctave(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	template<bool SwapBytesOrder>
	ElevationOctave& operator<<(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		int64_t _seed;
		float _frequency;
		float _weight;

		inputArchive >> _seed >> _frequency >> _weight;
		
		seed = _seed;
		frequency = _frequency;
		weight = _weight;

		return *this;
	}

	template<bool SwapBytesOrder>
	const ElevationOctave& operator>>(cd::TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << seed << frequency << weight;
		return *this;
	}

	int64_t seed;
	float frequency;
	float weight;
};

/*
 * Metadata regarding a single sector.This maps to row times column number of Mesh objects
 * where each is a TerrainQuad. See the struct for more details
 */
struct TerrainSectorMetadata
{
	explicit TerrainSectorMetadata()
		: numQuadsInX(0)
		, numQuadsInZ(0)
		, quadLenInX(0)
		, quadLenInZ(0)
	{}

	explicit TerrainSectorMetadata(
		uint16_t _numQuadsInX, 
		uint16_t _numQuadsInZ, 
		uint16_t _quadLenInX, 
		uint16_t _quadLenInZ)
		: numQuadsInX(_numQuadsInX)
		, numQuadsInZ(_numQuadsInZ)
		, quadLenInX(_quadLenInX)
		, quadLenInZ(_quadLenInZ)
	{}

	template<bool SwapBytesOrder>
	explicit TerrainSectorMetadata(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	template<bool SwapBytesOrder>
	TerrainSectorMetadata& operator<<(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint16_t _numQuadsInX;
		uint16_t _numQuadsInZ;
		uint16_t _quadLenInX;
		uint16_t _quadLenInZ;

		inputArchive >> _numQuadsInX >> _numQuadsInZ >> _quadLenInX >> _quadLenInZ;

		numQuadsInX = _numQuadsInX;
		numQuadsInZ = _numQuadsInZ;
		quadLenInX = _quadLenInX;
		quadLenInZ = _quadLenInZ;

		return *this;
	}

	template<bool SwapBytesOrder>
	const TerrainSectorMetadata& operator>>(cd::TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << numQuadsInX << numQuadsInZ << quadLenInX << quadLenInZ;
		return *this;
	}

	uint16_t numQuadsInX;	// row
	uint16_t numQuadsInZ;	// column
	uint16_t quadLenInX;	// width of a quad
	uint16_t quadLenInZ;	// height of a quad, note this is in 2D and in z-axis
};

/*
 * Quad is made up of 2 triangles T1 and T2 shown below:
 * A-----B
 * | T1 /|
 * |   / |
 * |  /  |
 * | / T2|
 * D-----C
 * 
 * Vertices are A, B, C and D
 */
struct TerrainQuad
{
	explicit TerrainQuad(
		uint32_t leftTri, uint32_t rightTri,
		uint32_t topLeft, uint32_t topRight, uint32_t bottomRight, uint32_t bottomLeft)
		: leftTriPolygonId(leftTri)
		, rightTriPolygonId(rightTri)
		, topLeftVertexId(topLeft)
		, topRightVertexId(topRight)
		, bottomRightVertexId(bottomRight)
		, bottomLeftVertexId(bottomLeft)
	{}

	template<bool SwapBytesOrder>
	explicit TerrainQuad(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	template<bool SwapBytesOrder>
	TerrainQuad& operator<<(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t leftTri;
		uint32_t rightTri;
		uint32_t topLeft;
		uint32_t topRight;
		uint32_t bottomRight;
		uint32_t bottomLeft;

		inputArchive >> leftTri >> rightTri >> topLeft >> topRight >> bottomRight >> bottomLeft;

		leftTriPolygonId = leftTri;
		rightTriPolygonId = rightTri;
		topLeftVertexId = topLeft;
		topRightVertexId = topRight;
		bottomRightVertexId = bottomRight;
		bottomLeftVertexId = bottomLeft;

		return *this;
	}

	template<bool SwapBytesOrder>
	const TerrainQuad& operator>>(cd::TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << leftTriPolygonId << rightTriPolygonId 
			<< topLeftVertexId << topRightVertexId << bottomRightVertexId << bottomLeftVertexId;
		return *this;
	}

	uint32_t leftTriPolygonId;		// T1
	uint32_t rightTriPolygonId;		// T2
	uint32_t topLeftVertexId;		// A
	uint32_t topRightVertexId;		// B
	uint32_t bottomRightVertexId;	// C
	uint32_t bottomLeftVertexId;	// D
};

/*
 * Metadata regarding an entire Terrain. It is broken into row times column number of
 * Sectors; see SectorMetadata for details. 
 */
struct TerrainMetadata
{
	explicit TerrainMetadata()
		: numSectorsInX(0)
		, numSectorsInZ(0)
		, minElevation(0)
		, maxElevation(0)
		, redistPow(1)
		, octaves()
	{}

	explicit TerrainMetadata(
		uint16_t _numSectorsInX, 
		uint16_t _numSectorsInZ, 
		int32_t _minElevation,
		int32_t _maxElevation,
		float _redistPow,
		std::vector<ElevationOctave>& _octaves)
		: numSectorsInX(_numSectorsInX)
		, numSectorsInZ(_numSectorsInZ)
		, minElevation(_minElevation)
		, maxElevation(_maxElevation)
		, redistPow(_redistPow)
		, octaves(std::move(_octaves))
	{}

	template<bool SwapBytesOrder>
	explicit TerrainMetadata(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	template<bool SwapBytesOrder>
	TerrainMetadata& operator<<(cd::TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint16_t _numSectorsInX;
		uint16_t _numSectorsInZ;
		int32_t _minElevation;
		int32_t _maxElevation;
		float _redistPow;
		size_t octaveSize;

		inputArchive >> _numSectorsInX >> _numSectorsInZ >> _minElevation >> _maxElevation >> _redistPow >> octaveSize;

		numSectorsInX = _numSectorsInX;
		numSectorsInZ = _numSectorsInZ;
		minElevation = _minElevation;
		maxElevation = _maxElevation;
		redistPow = _redistPow;
		octaves.resize(octaveSize);
		inputArchive.ImportBuffer(octaves.data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TerrainMetadata& operator>>(cd::TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << numSectorsInX << numSectorsInZ
			<< minElevation << maxElevation << redistPow << octaves.size();
		outputArchive.ExportBuffer(octaves.data(), octaves.size());
		return *this;
	}

	uint16_t numSectorsInX;					// row
	uint16_t numSectorsInZ;					// column
	int32_t minElevation;					// the minimum elevation (can be negative) of the entire terrain
	int32_t maxElevation;					// the maximum elevation of the entire terrain
	float redistPow;						// used for elevation smoothing; elevation is raised to redistPow power
	std::vector<ElevationOctave> octaves;	// Set of weights and seeds at powers of 2 frequencies
};

}