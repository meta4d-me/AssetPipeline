#include "VertexFormatImpl.h"

namespace cd
{

VertexFormatImpl::VertexFormatImpl(const VertexFormatImpl& rhs)
{
	this->operator=(rhs);
}

VertexFormatImpl& VertexFormatImpl::operator=(const VertexFormatImpl& rhs)
{
	m_vertexLayouts = rhs.m_vertexLayouts;
	return *this;
}

void VertexFormatImpl::AddVertexAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count)
{
	m_vertexLayouts.push_back(VertexAttributeLayout{ .vertexAttributeType = attributeType,
		.attributeValueType = valueType,
		.attributeCount = count });
}

void VertexFormatImpl::AddVertexAttributeLayout(VertexAttributeLayout vertexLayout)
{
	m_vertexLayouts.emplace_back(cd::MoveTemp(vertexLayout));
}

const VertexAttributeLayout* VertexFormatImpl::GetVertexAttributeLayout(VertexAttributeType attributeType) const
{
	for (const auto& vertexLayout : m_vertexLayouts)
	{
		if (attributeType == vertexLayout.vertexAttributeType)
		{
			return &vertexLayout;
		}
	}

	return nullptr;
}

bool VertexFormatImpl::Contains(VertexAttributeType attributeType) const
{
	return GetVertexAttributeLayout(attributeType) != nullptr;
}

bool VertexFormatImpl::IsCompatiableTo(const VertexFormatImpl& other) const
{
	if (other.m_vertexLayouts.empty())
	{
		return m_vertexLayouts.empty();
	}

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
		else if (AttributeValueType::Int16 == vertexLayout.attributeValueType)
		{
			valueTypeSize = sizeof(int16_t);
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