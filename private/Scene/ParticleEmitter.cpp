#include "ParticleEmitterImpl.h"

#include "Scene/ParticleEmitter.h"

namespace cd
{

ParticleEmitter::ParticleEmitter(InputArchive& inputArchive)
{
    m_pParticleEmitterImpl = new ParticleEmitterImpl(inputArchive);
}

ParticleEmitter::ParticleEmitter(InputArchiveSwapBytes& inputArchive)
{
    m_pParticleEmitterImpl = new ParticleEmitterImpl(inputArchive);
}

ParticleEmitter::ParticleEmitter(ParticleEmitterID id, const char* pName)
{
    m_pParticleEmitterImpl = new ParticleEmitterImpl(id, cd::MoveTemp(pName));
}

ParticleEmitter::ParticleEmitter(ParticleEmitter&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

ParticleEmitter& ParticleEmitter::operator=(ParticleEmitter&& rhs)
{
    std::swap(m_pParticleEmitterImpl, rhs.m_pParticleEmitterImpl);
    return *this;
}

ParticleEmitter::~ParticleEmitter()
{
    if (m_pParticleEmitterImpl)
    {
        delete m_pParticleEmitterImpl;
        m_pParticleEmitterImpl = nullptr;
    }
}

PIMPL_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
PIMPL_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Position);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Velocity);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Accelerate);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, Color);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, FixedRotation);
PIMPL_COMPLEX_TYPE_APIS(ParticleEmitter, FixedScale);
PIMPL_STRING_TYPE_APIS(ParticleEmitter, Name);

ParticleEmitter& ParticleEmitter::operator<<(InputArchive& inputArchive)
{
    *m_pParticleEmitterImpl << inputArchive;
    return *this;
}

ParticleEmitter& ParticleEmitter::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pParticleEmitterImpl << inputArchive;
    return *this;
}

const ParticleEmitter& ParticleEmitter::operator>>(OutputArchive& outputArchive) const
{
    *m_pParticleEmitterImpl >> outputArchive;
    return *this;
}

const ParticleEmitter& ParticleEmitter::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pParticleEmitterImpl >> outputArchive;
    return *this;
}

}