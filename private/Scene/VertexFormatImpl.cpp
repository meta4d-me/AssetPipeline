#include "VertexFormatImpl.h"

namespace cd
{

void VertexFormatImpl::AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count)
{
	m_vertexLayouts.push_back(VertexAttributeLayout{ .vertexAttributeType = attributeType,
		.attributeValueType = valueType,
		.attributeCount = count });
}

bool VertexFormatImpl::Contains(VertexAttributeType attributeType) const
{
	for (const auto& vertexLayout : m_vertexLayouts)
	{
		if (attributeType == vertexLayout.vertexAttributeType)
		{
			return true;
		}
	}

	return false;
}

bool VertexFormatImpl::IsCompatiableTo(const VertexFormatImpl& other) const
{
	for (const auto& vertexLayout : other.m_vertexLayouts)
	{
		if (!Contains(vertexLayout.vertexAttributeType))
		{
			return false;
		}
	}

	return true;
}

uint32_t VertexFormatImpl::GetStride() const
{
	uint32_t stride = 0U;
	for (const auto& vertexLayout : m_vertexLayouts)
	{
		uint32_t valueTypeSize;
		if (AttributeValueType::Uint8 == vertexLayout.attributeValueType)
		{
			valueTypeSize = sizeof(uint8_t);
		}
		else if (AttributeValueType::Uint16 == vertexLayout.attributeValueType)
		{
			valueTypeSize = sizeof(uint16_t);
		}
		else if (AttributeValueType::Float == vertexLayout.attributeValueType)
		{
			valueTypeSize = sizeof(float);
		}
		else
		{
			valueTypeSize = sizeof(float);
		}

		stride += valueTypeSize * vertexLayout.attributeCount;
	}

	return stride;
}

}