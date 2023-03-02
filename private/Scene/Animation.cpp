#include "Scene/Animation.h"
#include "AnimationImpl.h"

namespace cd
{

Animation::Animation(InputArchive& inputArchive)
{
    m_pAnimationImpl = new AnimationImpl(inputArchive);
}

Animation::Animation(InputArchiveSwapBytes& inputArchive)
{
    m_pAnimationImpl = new AnimationImpl(inputArchive);
}

Animation::Animation(AnimationID id, std::string name)
{
    m_pAnimationImpl = new AnimationImpl(id, cd::MoveTemp(name));
}

Animation::Animation(Animation&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Animation& Animation::operator=(Animation&& rhs)
{
    std::swap(m_pAnimationImpl, rhs.m_pAnimationImpl);
    return *this;
}

Animation::~Animation()
{
    if (m_pAnimationImpl)
    {
        delete m_pAnimationImpl;
        m_pAnimationImpl = nullptr;
    }
}

void Animation::Init(AnimationID id, std::string name)
{
    m_pAnimationImpl->Init(id, cd::MoveTemp(name));
}

const AnimationID& Animation::GetID() const
{
    return m_pAnimationImpl->GetID();
}

void Animation::SetName(std::string name)
{
    return m_pAnimationImpl->SetName(cd::MoveTemp(name));
}

const char* Animation::GetName() const
{
    return m_pAnimationImpl->GetName().c_str();
}

void Animation::SetDuration(float duration)
{
    m_pAnimationImpl->SetDuration(duration);
}

float Animation::GetDuration() const
{
    return m_pAnimationImpl->GetDuration();
}

void Animation::SetTicksPerSecond(float ticksPerSecond)
{
    m_pAnimationImpl->SetTicksPerSecond(ticksPerSecond);
}

float Animation::GetTicksPerSecnod() const
{
    return m_pAnimationImpl->GetTicksPerSecnod();
}

void Animation::AddBoneTrackID(uint32_t trackID)
{
    m_pAnimationImpl->AddBoneTrackID(trackID);
}

uint32_t Animation::GetBoneTrackCount() const
{
    return m_pAnimationImpl->GetBoneTrackCount();
}

std::vector<TrackID>& Animation::GetBoneTrackIDs()
{
    return m_pAnimationImpl->GetBoneTrackIDs();
}

const std::vector<TrackID>& Animation::GetBoneTrackIDs() const
{
    return m_pAnimationImpl->GetBoneTrackIDs();
}

Animation& Animation::operator<<(InputArchive& inputArchive)
{
    *m_pAnimationImpl << inputArchive;
    return *this;
}

Animation& Animation::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pAnimationImpl << inputArchive;
    return *this;
}

const Animation& Animation::operator>>(OutputArchive& outputArchive) const
{
    *m_pAnimationImpl >> outputArchive;
    return *this;
}

const Animation& Animation::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pAnimationImpl >> outputArchive;
    return *this;
}

}