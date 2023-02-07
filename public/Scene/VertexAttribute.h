#pragma once

#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"

#include <cstdint>
#include <type_traits>

namespace cd
{

static constexpr uint32_t MaxUVSetCount = 4U;
static constexpr uint32_t MaxColorSetCount = 4U;
static constexpr uint32_t MaxBoneInfluenceCount = 8U;
using VertexWeight = float;

// We expect to use triangulated mesh data in game engine.
using Polygon = TVector<VertexID, 3>;

enum class VertexAttributeType : uint8_t
{
	Position,
	Normal,
	Tangent,
	Bitangent,
	UV,
	Color,
};

enum class AttributeValueType : uint8_t
{
	Uint8,
	Float,
	Uint16,
};

template<typename T>
static constexpr AttributeValueType GetAttributeValueType()
{
	if constexpr (std::is_same<T, uint8_t>())
	{
		return AttributeValueType::Uint8;
	}
	else if constexpr (std::is_same<T, uint16_t>())
	{
		return AttributeValueType::Uint16;
	}
	else if constexpr (std::is_same<T, float>())
	{
		return AttributeValueType::Float;
	}
	else
	{
		return AttributeValueType::Float;
	}
}

struct VertexAttributeLayout
{
	VertexAttributeType vertexAttributeType;
	AttributeValueType attributeValueType;
	uint8_t attributeCount;
	uint8_t padding;
};

}