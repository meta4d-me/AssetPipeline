#include "TransformImpl.h"

namespace cd
{

TransformImpl::TransformImpl(TransformID transformID, Matrix4x4 transformation)
{
	Init(transformID, transformation);
}

void TransformImpl::Init(TransformID transformID, Matrix4x4 transformation)
{
	m_id = transformID;
	m_transformation = cd::MoveTemp(transformation);
}

}