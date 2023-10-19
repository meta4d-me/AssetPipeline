#include "Scene/ParticEmitter.h"
#include "ParticleEmitterImpl.h"

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
    m_pParticleEmitterImpl = new ParticleEmitterImpl(id, pName);
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

PIMPL_ID_APIS(ParticleEmitter);
PIMPL_NAME_APIS(ParticleEmitter);

void ParticleEmitter::SetPosition(cd::Vec3f position)
{
    m_pParticleEmitterImpl->SetPosition(cd::MoveTemp(position));
}

Vec3f& ParticleEmitter::GetPosition()
{
    return m_pParticleEmitterImpl->GetPosition();
}

const cd::Vec3f& ParticleEmitter::GetPosition() const
{
    return m_pParticleEmitterImpl->GetPosition();
}

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