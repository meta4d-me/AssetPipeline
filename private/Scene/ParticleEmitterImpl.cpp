#include "ParticleEmitterImpl.h"

namespace cd
{

ParticleEmitterImpl::ParticleEmitterImpl(ParticleEmitterID id, std::string name)
{
	Init(id, cd::MoveTemp(name));
}

void ParticleEmitterImpl::Init(ParticleEmitterID id, std::string name)
{
	m_id = id;
	m_name = cd::MoveTemp(name);
}

}