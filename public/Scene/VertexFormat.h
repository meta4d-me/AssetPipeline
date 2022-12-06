#pragma once

#include "Core/ISerializable.hpp"

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
	else if (std::is_same<T, float>())
	{
		return AttributeValueType::Float;
	}
	else
	{
		return AttributeValueType::Float;
	}
}

class VertexFormat : public ISerializable
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

	// ISerializable
	virtual void ImportBinary(std::ifstream& fin) override;
	virtual void ExportBinary(std::ofstream& fout) const override;

private:
	std::vector<VertexAttributeLayout> m_vertexLayout;
};

}