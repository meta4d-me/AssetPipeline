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
	DECLARE_SCENE_CLASS(ParticleEmitter);
	explicit ParticleEmitter(ParticleEmitterID id, const char* pName);

	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, MeshID);
	EXPORT_SIMPLE_TYPE_APIS(ParticleEmitter, MaxCount);
	EXPORT_STRING_TYPE_APIS(ParticleEmitter, Name);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Position);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Velocity);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Accelerate);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, Color);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedRotation);
	EXPORT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedScale);
};

}