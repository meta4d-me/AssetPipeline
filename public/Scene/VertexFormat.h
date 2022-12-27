#pragma once

#include "Base/Export.h"
#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/VertexAttribute.h"

#include <vector>

namespace cd
{

class VertexFormatImpl;

class TOOL_API VertexFormat
{
public:
	explicit VertexFormat();
	VertexFormat(const VertexFormat&) = delete;
	VertexFormat& operator=(const VertexFormat&) = delete;
	VertexFormat(VertexFormat&&);
	VertexFormat& operator=(VertexFormat&&);
	~VertexFormat();

	void AddAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count);
	const std::vector<VertexAttributeLayout>& GetVertexLayout() const;

	// Returns if vertex format contains vertex attribute type.
	bool Contains(VertexAttributeType attributeType) const;

	VertexFormat& operator<<(InputArchive& inputArchive);
	VertexFormat& operator<<(InputArchiveSwapBytes& inputArchive);
	const VertexFormat& operator>>(OutputArchive& outputArchive) const;
	const VertexFormat& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	VertexFormatImpl* m_pVertexFormatImpl = nullptr;
};

}