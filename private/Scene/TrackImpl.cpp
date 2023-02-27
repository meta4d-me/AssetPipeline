#include "TrackImpl.h"

namespace cd
{

TrackImpl::TrackImpl(TrackID id, std::string name)
{
	Init(id, cd::MoveTemp(name));
}

void TrackImpl::Init(TrackID id, std::string name)
{
	m_id = id;
	m_name = cd::MoveTemp(name);
}

}