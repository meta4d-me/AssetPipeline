#include "LightImpl.h"

namespace cd
{

LightImpl::LightImpl(InputArchive& inputArchive)
{
	*this << inputArchive;
}

LightImpl::LightImpl(InputArchiveSwapBytes& inputArchive)
{
	*this << inputArchive;
}

LightImpl::LightImpl(LightID lightID, LightType type)
{
	Init(lightID, type);
}

void LightImpl::Init(LightID lightID, LightType type)
{
	m_id = lightID;
	m_type = type;
}

std::pair<float, float> LightImpl::CalculateScaleAndOffset(float innerAngle, float outerAngle) const
{
	const float cosInner = std::cos(Math::DegreeToRadian<float>(innerAngle));
	const float cosOuter = std::cos(Math::DegreeToRadian<float>(outerAngle));
	const float lightAngleScale = 1.0f / std::max(0.001f, cosInner - cosOuter);
	const float lightAngleOffeset = -cosOuter * lightAngleScale;

	return { lightAngleScale, lightAngleOffeset };
}

}
