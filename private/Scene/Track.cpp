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

void Track::SetID(TrackID id)
{
    m_pTrackImpl->SetID(id);
}

TrackID Track::GetID() const
{
    return m_pTrackImpl->GetID();
}

void Track::SetName(std::string name)
{
    return m_pTrackImpl->SetName(cd::MoveTemp(name));
}

const char* Track::GetName() const
{
    return m_pTrackImpl->GetName().c_str();
}

void Track::SetTranslationKeyCount(uint32_t keyCount)
{
    m_pTrackImpl->SetTranslationKeyCount(keyCount);
}

uint32_t Track::GetTranslationKeyCount() const
{
    return m_pTrackImpl->GetTranslationKeyCount();
}

void Track::SetTranslationKeys(std::vector<TranslationKey> keys)
{
    m_pTrackImpl->SetTranslationKeys(MoveTemp(keys));
}

std::vector<TranslationKey>& Track::GetTranslationKeys()
{
    return m_pTrackImpl->GetTranslationKeys();
}

const std::vector<TranslationKey>& Track::GetTranslationKeys() const
{
    return m_pTrackImpl->GetTranslationKeys();
}

void Track::SetRotationKeyCount(uint32_t keyCount)
{
    m_pTrackImpl->SetRotationKeyCount(keyCount);
}

uint32_t Track::GetRotationKeyCount() const
{
    return m_pTrackImpl->GetRotationKeyCount();
}

void Track::SetRotationKeys(std::vector<RotationKey> keys)
{
    m_pTrackImpl->SetRotationKeys(MoveTemp(keys));
}

std::vector<RotationKey>& Track::GetRotationKeys()
{
    return m_pTrackImpl->GetRotationKeys();
}

const std::vector<RotationKey>& Track::GetRotationKeys() const
{
    return m_pTrackImpl->GetRotationKeys();
}

void Track::SetScaleKeyCount(uint32_t keyCount)
{
    m_pTrackImpl->SetScaleKeyCount(keyCount);
}

uint32_t Track::GetScaleKeyCount() const
{
    return m_pTrackImpl->GetScaleKeyCount();
}

void Track::SetScaleKeys(std::vector<ScaleKey> keys)
{
    m_pTrackImpl->SetScaleKeys(MoveTemp(keys));
}

std::vector<ScaleKey>& Track::GetScaleKeys()
{
    return m_pTrackImpl->GetScaleKeys();
}

const std::vector<ScaleKey>& Track::GetScaleKeys() const
{
    return m_pTrackImpl->GetScaleKeys();
}

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