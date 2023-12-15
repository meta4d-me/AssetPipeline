#include "AnimationImpl.h"

namespace cd
{

AnimationImpl::AnimationImpl(AnimationID id, std::string name)
{
	Init(id, cd::MoveTemp(name));
}

void AnimationImpl::Init(AnimationID id, std::string name)
{
	SetID(id);
	SetName(cd::MoveTemp(name));
}

}