#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <stdint.h>

namespace cdtools
{

DEFINE_ENUM_WITH_NAMES(AlphaMapType, Texture, Elevation, Simplex2D);
DEFINE_ENUM_WITH_NAMES(AlphaMapChannel, Red, Green, Blue, Alpha);
DEFINE_ENUM_WITH_NAMES(AlphaMapBlendFunction, Step, Linear, SmoothStep, SmoothStepHigh);

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