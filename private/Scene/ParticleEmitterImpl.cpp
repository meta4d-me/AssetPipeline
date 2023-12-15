#include "ParticleEmitterImpl.h"

namespace cd
{

ParticleEmitterImpl::ParticleEmitterImpl(ParticleEmitterID id, std::string name)
{
	Init(id, MoveTemp(name));
}

void ParticleEmitterImpl::Init(ParticleEmitterID id, std::string name)
{
	SetID(id);
	SetName(MoveTemp(name));
}

}