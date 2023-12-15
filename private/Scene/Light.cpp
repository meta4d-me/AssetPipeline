#include "Scene/Light.h"
#include "LightImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Light);

Light::Light(LightID lightID, const LightType type)
{
	m_pLightImpl = new LightImpl(lightID, type);
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

}