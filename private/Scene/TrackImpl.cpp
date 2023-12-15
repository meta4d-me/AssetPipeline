#include "TrackImpl.h"

namespace cd
{

TrackImpl::TrackImpl(TrackID id, std::string name)
{
	Init(id, MoveTemp(name));
}

void TrackImpl::Init(TrackID id, std::string name)
{
	SetID(id);
	SetName(MoveTemp(name));
}

}