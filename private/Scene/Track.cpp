#include "Scene/Track.h"
#include "TrackImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Track);

Track::Track(TrackID id, std::string name)
{
    m_pTrackImpl = new TrackImpl(id, cd::MoveTemp(name));
}

void Track::Init(TrackID id, std::string name)
{
    m_pTrackImpl->Init(id, cd::MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Track, ID);
PIMPL_STRING_TYPE_APIS(Track, Name);
PIMPL_VECTOR_TYPE_APIS(Track, TranslationKey);
PIMPL_VECTOR_TYPE_APIS(Track, RotationKey);
PIMPL_VECTOR_TYPE_APIS(Track, ScaleKey);

}