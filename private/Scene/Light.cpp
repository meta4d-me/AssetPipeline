#include "Scene/Light.h"
#include "LightImpl.h"

namespace cd
{

Light::Light(InputArchive& inputArchive)
{
	m_pLightImpl = new LightImpl(inputArchive);
}

Light::Light(InputArchiveSwapBytes& inputArchive)
{
	m_pLightImpl = new LightImpl(inputArchive);
}

Light::Light(LightID lightID, const LightType type)
{
	m_pLightImpl = new LightImpl(lightID, type);
}

Light::Light(Light&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

Light& Light::operator=(Light&& rhs)
{
	std::swap(m_pLightImpl, rhs.m_pLightImpl);
	return *this;
}

Light::~Light()
{
	if (m_pLightImpl)
	{
		delete m_pLightImpl;
		m_pLightImpl = nullptr;
	}
}

void Light::Init(LightID lightID, LightType type)
{
	m_pLightImpl->Init(lightID, type);
}

std::pair<float, float> Light::CalculateScaleAndOffset(float innerAngle, float outerAngle) const
{
	return m_pLightImpl->CalculateScaleAndOffset(innerAngle, outerAngle);
}

PIMPL_SIMPLE_TYPE_APIS(Light, ID);
PIMPL_SIMPLE_TYPE_APIS(Light, Type);
PIMPL_SIMPLE_TYPE_APIS(Light, Intensity);
PIMPL_SIMPLE_TYPE_APIS(Light, Range);
PIMPL_SIMPLE_TYPE_APIS(Light, Radius);
PIMPL_SIMPLE_TYPE_APIS(Light, Width);
PIMPL_SIMPLE_TYPE_APIS(Light, Height);
PIMPL_SIMPLE_TYPE_APIS(Light, AngleScale);
PIMPL_SIMPLE_TYPE_APIS(Light, AngleOffset);
PIMPL_COMPLEX_TYPE_APIS(Light, Color);
PIMPL_COMPLEX_TYPE_APIS(Light, Position);
PIMPL_COMPLEX_TYPE_APIS(Light, Direction);
PIMPL_COMPLEX_TYPE_APIS(Light, Up);
PIMPL_STRING_TYPE_APIS(Light, Name);

Light& Light::operator<<(InputArchive& inputArchive)
{
	*m_pLightImpl << inputArchive;
	return *this;
}

Light& Light::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pLightImpl << inputArchive;
	return *this;
}

const Light& Light::operator>>(OutputArchive& outputArchive) const
{
	*m_pLightImpl >> outputArchive;
	return *this;
}

const Light& Light::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pLightImpl >> outputArchive;
	return *this;
}

}