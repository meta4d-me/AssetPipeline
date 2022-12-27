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

LightImpl::LightImpl(LightID lightID, const float type)
{
	Init(lightID, type);
}

void LightImpl::Init(LightID lightID, const float type)
{
	m_id = lightID;
	m_type = type;
}

const std::pair<float, float> LightImpl::CalculateScaleAndOffeset(const float innerAngle, const float outerAngle) const
{
	auto Angle2Radian = [](const float& angle) { return 3.1415926535f / 180.0f * angle; };
	const float cosInner = std::cos(Angle2Radian(innerAngle));
	const float cosOuter = std::cos(Angle2Radian(outerAngle));
	const float lightAngleScale = 1.0f / std::max(0.001f, cosInner - cosOuter);
	const float lightAngleOffeset = -cosOuter * lightAngleScale;

	return { lightAngleScale, lightAngleOffeset };
}

}
