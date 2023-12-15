#include "CameraImpl.h"

namespace cd
{

CameraImpl::CameraImpl(CameraID id, std::string name)
{
	Init(id, cd::MoveTemp(name));
}

void CameraImpl::Init(CameraID id, std::string name)
{
	SetID(id);
	SetName(cd::MoveTemp(name));
}

}