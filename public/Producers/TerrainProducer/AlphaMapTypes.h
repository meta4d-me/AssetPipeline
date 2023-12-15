#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <cstdint>

namespace cdtools
{

enum class AlphaMapType : uint8_t
{
	Texture = 0x0,
	Elevation = 0x1,
	Simplex2D = 0x2,
	Count
};

enum class AlphaMapChannel : uint8_t
{
	Red = 0x0,
	Green = 0x1,
	Blue = 0x2,
	Alpha = 0x3,
	Count
};

enum class AlphaMapBlendFunction : uint8_t 
{
	Step,
	Linear,
	SmoothStep,
	SmoothStepHigh,
	Count
};

template<typename T, typename = typename std::enable_if<std::is_arithmetic_v<T>, T>::type>
struct AlphaMapBlendRegion 
{
	T blendStart;
	T blendEnd;
};

template<typename T, AlphaMapType M, typename = typename std::enable_if<std::is_arithmetic_v<T>, T>::type>
struct AlphaMapDef
{
	const AlphaMapType type = M;
	AlphaMapBlendFunction blendFunction;
	AlphaMapBlendRegion<T> redGreenBlendRegion;
	AlphaMapBlendRegion<T> greenBlueBlendRegion;
	AlphaMapBlendRegion<T> blueAlphaBlendRegion;
	cd::TextureID alphaMapTextureId;
};

// Specializations
using ElevationAlphaMapDef = AlphaMapDef<int32_t, AlphaMapType::Elevation>;
using TextureAlphaMapDef = AlphaMapDef<unsigned char, AlphaMapType::Texture>;	// Assume RGBA format; don't need blend region; using unsighed char as filler
using NoiseAlphaMapDef = AlphaMapDef<float, AlphaMapType::Simplex2D>;

}