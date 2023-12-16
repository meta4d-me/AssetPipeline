#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ParticleEmitter.h"
#include "Scene/VertexFormat.h"
#include "Scene/Types.h"

namespace cd
{

class ParticleEmitterImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(ParticleEmitter);

	explicit ParticleEmitterImpl(ParticleEmitterID id, std::string name);
	void Init(ParticleEmitterID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
	IMPLEMENT_SIMPLE_TYPE_APIS(ParticleEmitter, MeshID);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, Position);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, Velocity);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, Accelerate);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, Color);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedRotation);
	IMPLEMENT_COMPLEX_TYPE_APIS(ParticleEmitter, FixedScale);
	IMPLEMENT_STRING_TYPE_APIS(ParticleEmitter, Name);

	template<bool SwapBytesOrder>
	ParticleEmitterImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t emitterID;
		std::string emitterName;
		uint32_t emitterType;
		uint32_t meshID;
		inputArchive >> emitterID >> emitterName >> emitterType >> meshID;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
		SetType(static_cast<ParticleEmitterType>(emitterType));
		SetMeshID(meshID);
		inputArchive >> GetPosition() >> GetVelocity() >> GetAccelerate()
			>> GetColor() >> GetFixedRotation() >> GetFixedScale();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint32_t>(GetType())
			<< GetMeshID().Data() << GetPosition() << GetVelocity() << GetAccelerate()
			<< GetColor() << GetFixedRotation() << GetFixedScale();
		return *this;
	}
};

}