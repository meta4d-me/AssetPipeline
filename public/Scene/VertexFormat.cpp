#include "VertexFormat.h"

namespace cd
{

void VertexFormat::AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count)
{
	m_vertexLayout.push_back(VertexAttributeLayout{ .vertexAttributeType = attributeType,
		.attributeValueType = valueType,
		.attributeCount = count });
}

void VertexFormat::ImportBinary(std::ifstream& fin)
{
	std::uint8_t vertexLayoutCount = static_cast<std::uint8_t>(m_vertexLayout.size());
	ImportData<>(fin, vertexLayoutCount);
	m_vertexLayout.resize(vertexLayoutCount);
	ImportDataBuffer(fin, m_vertexLayout.data());
}

void VertexFormat::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::uint8_t>(fout, static_cast<std::uint8_t>(m_vertexLayout.size()));
	ExportDataBuffer(fout, m_vertexLayout.data(), m_vertexLayout.size());
}

}