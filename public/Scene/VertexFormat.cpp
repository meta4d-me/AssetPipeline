#include "VertexFormat.h"

namespace cdtools
{

void VertexFormat::AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count)
{
	m_vertexLayout.push_back(VertexAttributeLayout{ .vertexAttributeType = attributeType,
		.attributeValueType = valueType,
		.attributeCount = count });
}

void VertexFormat::ImportBinary(std::ifstream& fin)
{
	ImportDataBuffer(fin, m_vertexLayout.data());
}

void VertexFormat::ExportBinary(std::ofstream& fout) const
{
	ExportDataBuffer(fout, m_vertexLayout.data(), m_vertexLayout.size());
}

}