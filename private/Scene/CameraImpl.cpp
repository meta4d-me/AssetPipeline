#include "CameraImpl.h"

namespace cd
{

CameraImpl::CameraImpl(CameraID id, std::string name)
{
	Init(id, cd::MoveTemp(name));
}

void CameraImpl::Init(CameraID id, std::string name)
{
	m_id = id;
	m_name = cd::MoveTemp(name);
}

}