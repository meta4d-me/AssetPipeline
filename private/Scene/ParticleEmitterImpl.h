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
	ParticleEmitterImpl() = delete;
	template<bool SwapBytesOrder>
	explicit ParticleEmitterImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit ParticleEmitterImpl(ParticleEmitterID id, std::string name);
	ParticleEmitterImpl(const ParticleEmitterImpl&) = default;
	ParticleEmitterImpl& operator=(const ParticleEmitterImpl&) = default;
	ParticleEmitterImpl(ParticleEmitterImpl&&) = default;
	ParticleEmitterImpl& operator=(ParticleEmitterImpl&&) = default;
	~ParticleEmitterImpl() = default;

	void Init(ParticleEmitterID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(ParticleEmitter, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(ParticleEmitter, Type);
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
		inputArchive >> emitterID >> emitterName >> emitterType;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
		SetType(static_cast<ParticleEmitterType>(emitterType));
		inputArchive >> GetPosition() >> GetVelocity() >> GetAccelerate()
			>> GetColor() >> GetFixedRotation() >> GetFixedScale();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint32_t>(GetType())
			<< GetPosition() << GetVelocity() << GetAccelerate()
			<< GetColor() << GetFixedRotation() << GetFixedScale();
		return *this;
	}
};

}