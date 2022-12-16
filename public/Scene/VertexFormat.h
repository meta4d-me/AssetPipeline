#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"

#include <fstream>
#include <inttypes.h>
#include <vector>

namespace cd
{

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

class VertexFormat
{
public:
	struct VertexAttributeLayout
	{
		VertexAttributeType vertexAttributeType;
		AttributeValueType attributeValueType;
		uint8_t attributeCount;
		uint8_t padding;
	};

public:
	explicit VertexFormat() = default;
	VertexFormat(const VertexFormat&) = delete;
	VertexFormat& operator=(const VertexFormat&) = delete;
	VertexFormat(VertexFormat&&) = default;
	VertexFormat& operator=(VertexFormat&&) = default;
	~VertexFormat() = default;

	void AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count);
	const std::vector<VertexAttributeLayout>& GetVertexLayout() const { return m_vertexLayout; }

	template<bool SwapBytesOrder>
	VertexFormat& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::uint8_t vertexLayoutCount;
		inputArchive >> vertexLayoutCount;
		m_vertexLayout.resize(static_cast<size_t>(vertexLayoutCount));
		inputArchive.ImportBuffer(m_vertexLayout.data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const VertexFormat& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << static_cast<std::uint8_t>(m_vertexLayout.size());
		outputArchive.ExportBuffer(m_vertexLayout.data(), m_vertexLayout.size());

		return *this;
	}

private:
	std::vector<VertexAttributeLayout> m_vertexLayout;
};

}