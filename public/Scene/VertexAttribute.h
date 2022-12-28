#pragma once

#include <cstdint>
#include <type_traits>

namespace cd
{

static constexpr uint32_t MaxUVSetNumber = 4U;
static constexpr uint32_t MaxColorSetNumber = 4U;

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
};

template<typename T>
static constexpr AttributeValueType GetAttributeValueType()
{
	if constexpr (std::is_same<T, uint8_t>())
	{
		return AttributeValueType::Uint8;
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