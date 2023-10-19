#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

namespace cd
{

class ParticleEmitterImpl;

class CORE_API ParticleEmitter final
{
public:
	static const char* GetClassName() { return "ParticleEmitter"; }

public:
	ParticleEmitter() = delete;
	explicit ParticleEmitter(InputArchive& inputArchive);
	explicit ParticleEmitter(InputArchiveSwapBytes& inputArchive);
	explicit ParticleEmitter(ParticleEmitterID id, const char* pName);
	ParticleEmitter(const ParticleEmitter&) = delete;
	ParticleEmitter& operator=(const ParticleEmitter&) = delete;
	ParticleEmitter(ParticleEmitter&&);
	ParticleEmitter& operator=(ParticleEmitter&&);
	~ParticleEmitter();

	EXPORT_OBJECT_ID_APIS(ParticleEmitterID);
	EXPORT_NAME_APIS();

	void SetPosition(cd::Vec3f position);
	Vec3f& GetPosition();
	const cd::Vec3f& GetPosition() const;

	ParticleEmitter& operator<<(InputArchive& inputArchive);
	ParticleEmitter& operator<<(InputArchiveSwapBytes& inputArchive);
	const ParticleEmitter& operator>>(OutputArchive& outputArchive) const;
	const ParticleEmitter& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	ParticleEmitterImpl* m_pParticleEmitterImpl = nullptr;
};

}