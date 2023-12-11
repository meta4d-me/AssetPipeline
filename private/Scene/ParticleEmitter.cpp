#include "Scene/ParticleEmitter.h"
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

PIMPL_ID_APIS(ParticleEmitter);
PIMPL_NAME_APIS(ParticleEmitter);

void ParticleEmitter::SetType(int type)
{
    m_pParticleEmitterImpl->SetType(cd::MoveTemp(type));
}

int& ParticleEmitter::GetType()
{
    return m_pParticleEmitterImpl->GetType();
}

const int& ParticleEmitter::GetType() const
{
    return m_pParticleEmitterImpl->GetType();
}

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

void ParticleEmitter::SetVelocity(cd::Vec3f velocity)
{
    m_pParticleEmitterImpl->SetVelocity(cd::MoveTemp(velocity));
}

Vec3f& ParticleEmitter::GetVelocity()
{
    return m_pParticleEmitterImpl->GetVelocity();
}

const cd::Vec3f& ParticleEmitter::GetVelocity() const
{
    return m_pParticleEmitterImpl->GetVelocity();
}

void ParticleEmitter::SetAccelerate(cd::Vec3f accelerate)
{
    m_pParticleEmitterImpl->SetAccelerate(cd::MoveTemp(accelerate));
}

Vec3f& ParticleEmitter::GetAccelerate()
{
    return m_pParticleEmitterImpl->GetAccelerate();
}

const cd::Vec3f& ParticleEmitter::GetAccelerate() const
{
    return m_pParticleEmitterImpl->GetAccelerate();
}

void ParticleEmitter::SetColor(cd::Vec4f color)
{
    m_pParticleEmitterImpl->SetColor(cd::MoveTemp(color));
}

Vec4f& ParticleEmitter::GetColor()
{
    return m_pParticleEmitterImpl->GetColor();
}

const cd::Vec4f& ParticleEmitter::GetColor() const
{
    return m_pParticleEmitterImpl->GetColor();
}

void ParticleEmitter::SetFixedRotation(cd::Vec3f rotation)
{
    m_pParticleEmitterImpl->SetFixedRotation(cd::MoveTemp(rotation));
}

Vec3f& ParticleEmitter::GetFixedRotation()
{
    return m_pParticleEmitterImpl->GetFixedRotation();
}

const cd::Vec3f& ParticleEmitter::GetFixedRotation() const
{
    return m_pParticleEmitterImpl->GetFixedRotation();
}

void ParticleEmitter::SetFixedScale(cd::Vec3f scale)
{
    m_pParticleEmitterImpl->SetFixedScale(cd::MoveTemp(scale));
}

Vec3f& ParticleEmitter::GetFixedScale()
{
    return m_pParticleEmitterImpl->GetFixedScale();
}

const cd::Vec3f& ParticleEmitter::GetFixedScale() const
{
    return m_pParticleEmitterImpl->GetFixedScale();
}

void ParticleEmitter::SetTypeName(std::string name)
{
    m_pParticleEmitterImpl->SetTypeName(cd::MoveTemp(name));
}

std::string& ParticleEmitter::GetTypeName()
{
    return m_pParticleEmitterImpl->GetTypeName();
}

const std::string& ParticleEmitter::GetTypeName() const
{
    return m_pParticleEmitterImpl->GetTypeName();
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