#include "Scene/Track.h"
#include "TrackImpl.h"

namespace cd
{

Track::Track(InputArchive& inputArchive)
{
    m_pTrackImpl = new TrackImpl(inputArchive);
}

Track::Track(InputArchiveSwapBytes& inputArchive)
{
    m_pTrackImpl = new TrackImpl(inputArchive);
}

Track::Track(TrackID id, std::string name)
{
    m_pTrackImpl = new TrackImpl(id, cd::MoveTemp(name));
}

Track::Track(Track&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Track& Track::operator=(Track&& rhs)
{
    std::swap(m_pTrackImpl, rhs.m_pTrackImpl);
    return *this;
}

Track::~Track()
{
    if (m_pTrackImpl)
    {
        delete m_pTrackImpl;
        m_pTrackImpl = nullptr;
    }
}

void Track::Init(TrackID id, std::string name)
{
    m_pTrackImpl->Init(id, cd::MoveTemp(name));
}

PIMPL_ID_APIS(Track);
PIMPL_NAME_APIS(Track);
PIMPL_VECTOR_DATA_APIS(Track, TranslationKey);
PIMPL_VECTOR_DATA_APIS(Track, RotationKey);
PIMPL_VECTOR_DATA_APIS(Track, ScaleKey);

Track& Track::operator<<(InputArchive& inputArchive)
{
    *m_pTrackImpl << inputArchive;
    return *this;
}

Track& Track::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pTrackImpl << inputArchive;
    return *this;
}

const Track& Track::operator>>(OutputArchive& outputArchive) const
{
    *m_pTrackImpl >> outputArchive;
    return *this;
}

const Track& Track::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pTrackImpl >> outputArchive;
    return *this;
}

}