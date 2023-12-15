#pragma once

#include "Math/Vector.hpp"

#include <cstdint>
#include <type_traits>

namespace cd
{

static constexpr uint32_t MaxUVSetCount = 4U;
static constexpr uint32_t MaxColorSetCount = 4U;
static constexpr uint32_t MaxBoneInfluenceCount = 8U;
using VertexWeight = float;

enum class VertexAttributeType : uint8_t
{
	Position,
	Normal,
	Tangent,
	Bitangent,
	UV,
	Color,
	BoneWeight,
	BoneIndex,
};

enum class AttributeValueType : uint8_t
{
	Uint8,
	Float,
	Int16,
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
		return AttributeValueType::Int16;
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

enum class ConvertStrategy
{
	// Mesh faces will split along edges so uv/normal data can be same with edited source mesh. Vertex positions maybe duplicated.
	ShadingFirst,
	// Mesh vertex uv/normal data will be averaged of corner uv/normal data. Topology will be preserved.
	TopologyFirst,
	// Only convert boundary for special purpose.
	BoundaryOnly,
};

}