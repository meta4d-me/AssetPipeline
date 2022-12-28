#pragma once

#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/VertexAttribute.h"

#include <vector>

namespace cd
{

class VertexFormatImpl
{
public:
	explicit VertexFormatImpl() = default;
	VertexFormatImpl(const VertexFormatImpl&) = delete;
	VertexFormatImpl& operator=(const VertexFormatImpl&) = delete;
	VertexFormatImpl(VertexFormatImpl&&) = default;
	VertexFormatImpl& operator=(VertexFormatImpl&&) = default;
	~VertexFormatImpl() = default;

	void AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count);
	const std::vector<VertexAttributeLayout>& GetVertexLayout() const { return m_vertexLayouts; }

	// Returns if vertex format contains vertex attribute type.
	bool Contains(VertexAttributeType attributeType) const;

	template<bool SwapBytesOrder>
	VertexFormatImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::uint8_t vertexLayoutCount;
		inputArchive >> vertexLayoutCount;
		m_vertexLayouts.resize(static_cast<size_t>(vertexLayoutCount));
		inputArchive.ImportBuffer(m_vertexLayouts.data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const VertexFormatImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << static_cast<std::uint8_t>(m_vertexLayouts.size());
		outputArchive.ExportBuffer(m_vertexLayouts.data(), m_vertexLayouts.size());

		return *this;
	}

private:
	std::vector<VertexAttributeLayout> m_vertexLayouts;
};

}