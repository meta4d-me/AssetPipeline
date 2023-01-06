#include "TransformImpl.h"

namespace cd
{

TransformImpl::TransformImpl(TransformID transformID)
{
	Init(transformID);
}

void TransformImpl::Init(TransformID transformID)
{
	m_id = transformID;
}

}