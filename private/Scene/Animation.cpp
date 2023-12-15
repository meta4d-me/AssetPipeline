#include "Scene/Animation.h"
#include "AnimationImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Animation);

Animation::Animation(AnimationID id, std::string name)
{
    m_pAnimationImpl = new AnimationImpl(id, cd::MoveTemp(name));
}

void Animation::Init(AnimationID id, std::string name)
{
    m_pAnimationImpl->Init(id, cd::MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Animation, ID);
PIMPL_SIMPLE_TYPE_APIS(Animation, Duration);
PIMPL_SIMPLE_TYPE_APIS(Animation, TicksPerSecond);
PIMPL_VECTOR_TYPE_APIS(Animation, BoneTrackID);
PIMPL_STRING_TYPE_APIS(Animation, Name);

}