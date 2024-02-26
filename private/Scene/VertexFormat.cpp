#include "Scene/VertexFormat.h"
#include "VertexFormatImpl.h"

namespace cd
{

VertexFormat::VertexFormat()
{
	m_pVertexFormatImpl = new VertexFormatImpl();
}

VertexFormat::VertexFormat(const VertexFormat& rhs)
{
	*this = rhs;
}

VertexFormat& VertexFormat::operator=(const VertexFormat& rhs)
{
	m_pVertexFormatImpl = new VertexFormatImpl();
	*m_pVertexFormatImpl = *rhs.m_pVertexFormatImpl;
	return *this;
}

VertexFormat::VertexFormat(VertexFormat&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

VertexFormat& VertexFormat::operator=(VertexFormat&& rhs)
{
	std::swap(m_pVertexFormatImpl, rhs.m_pVertexFormatImpl);
	return *this;
}

VertexFormat::~VertexFormat()
{
	if (m_pVertexFormatImpl)
	{
		delete m_pVertexFormatImpl;
		m_pVertexFormatImpl = nullptr;
	}
}

void VertexFormat::AddVertexAttributeLayout(VertexAttributeType attributeType, AttributeValueType valueType, uint8_t count)
{
	m_pVertexFormatImpl->AddVertexAttributeLayout(attributeType, valueType, count);
}

void VertexFormat::AddVertexAttributeLayout(VertexAttributeLayout vertexLayout)
{
	m_pVertexFormatImpl->AddVertexAttributeLayout(cd::MoveTemp(vertexLayout));
}

const VertexAttributeLayout* VertexFormat::GetVertexAttributeLayout(VertexAttributeType attributeType) const
{
	return m_pVertexFormatImpl->GetVertexAttributeLayout(attributeType);
}

const std::vector<VertexAttributeLayout>& VertexFormat::GetVertexAttributeLayouts() const
{
	return m_pVertexFormatImpl->GetVertexAttributeLayouts();
}

bool VertexFormat::Contains(VertexAttributeType attributeType) const
{
	return m_pVertexFormatImpl->Contains(attributeType);
}

bool VertexFormat::IsCompatiableTo(const VertexFormat& other) const
{
	return m_pVertexFormatImpl->IsCompatiableTo(*other.m_pVertexFormatImpl);
}

uint32_t VertexFormat::GetStride() const
{
	return m_pVertexFormatImpl->GetStride();
}

VertexFormat& VertexFormat::operator<<(InputArchive& inputArchive)
{
	*m_pVertexFormatImpl << inputArchive;
	return *this;
}

VertexFormat& VertexFormat::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pVertexFormatImpl << inputArchive;
	return *this;
}

const VertexFormat& VertexFormat::operator>>(OutputArchive& outputArchive) const
{
	*m_pVertexFormatImpl >> outputArchive;
	return *this;
}

const VertexFormat& VertexFormat::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pVertexFormatImpl >> outputArchive;
	return *this;
}

}