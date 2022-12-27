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

}