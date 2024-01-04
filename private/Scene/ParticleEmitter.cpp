#include "ParticleEmitterImpl.h"

#include "Scene/ParticleEmitter.h"

namespace cd
{

PIMPL_SCENE_CLASS(ParticleEmitter);

ParticleEmitter::ParticleEmitter(ParticleEmitterID id, const char* pName)
{
    m_pParticleEmitterImpl = new ParticleEmitterImpl(id, cd::MoveTemp(pName));
}

PIMPL_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
PIMPL_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
PIMPL_SIMPLE_TYPE_APIS(ParticleEmitter, MeshID);
PIMPL_STRING_TYPE_APIS(ParticleEmitter, Name);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Position);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Velocity);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Accelerate);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Color);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, FixedRotation);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, FixedScale);

}