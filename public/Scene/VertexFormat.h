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

class CORE_API VertexFormat final
{
public:
	VertexFormat();
	VertexFormat(const VertexFormat&);
	VertexFormat& operator=(const VertexFormat&);
	VertexFormat(VertexFormat&&);
	VertexFormat& operator=(VertexFormat&&);
	~VertexFormat();

	void AddVertexAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count);
	void AddVertexAttributeLayout(VertexAttributeLayout vertexLayout);
	const VertexAttributeLayout* GetVertexAttributeLayout(VertexAttributeType attributeType) const;
	const std::vector<VertexAttributeLayout>& GetVertexAttributeLayouts() const;

	// Returns if vertex format contains vertex attribute type.
	bool Contains(VertexAttributeType attributeType) const;

	bool IsCompatiableTo(const VertexFormat& other) const;

	uint32_t GetStride() const;

	VertexFormat& operator<<(InputArchive& inputArchive);
	VertexFormat& operator<<(InputArchiveSwapBytes& inputArchive);
	const VertexFormat& operator>>(OutputArchive& outputArchive) const;
	const VertexFormat& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	VertexFormatImpl* m_pVertexFormatImpl = nullptr;
};

}