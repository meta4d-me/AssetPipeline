#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

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

	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Position);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Velocity);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Accelerate);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Color);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedRotation);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedScale);
	EXPORT_STRING_TYPE_APIS(ParticleEmitter, Name);

	ParticleEmitter& operator<<(InputArchive& inputArchive);
	ParticleEmitter& operator<<(InputArchiveSwapBytes& inputArchive);
	const ParticleEmitter& operator>>(OutputArchive& outputArchive) const;
	const ParticleEmitter& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	ParticleEmitterImpl* m_pParticleEmitterImpl = nullptr;
};

}