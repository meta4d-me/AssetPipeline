#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

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

	IMPLEMENT_ID_APIS(ParticleEmitterID, m_id);
	IMPLEMENT_NAME_APIS(m_name);

	//Position
	void SetPosition(Vec3f position) { m_Position = MoveTemp(position); }
	Vec3f& GetPosition() { return m_Position; }
	const Vec3f& GetPosition() const { return m_Position; }

	//Veloctity
	void SetVelocity(Vec3f velocity) { m_Velocity = MoveTemp(velocity); }
	Vec3f& GetVelocity() { return m_Velocity; }
	const Vec3f& GetVelocity() const { return m_Velocity; }

	//Accelerate
	void SetAccelerate(Vec3f accelerate) { m_Accelerate = MoveTemp(accelerate); }
	Vec3f& GetAccelerate() { return m_Accelerate; }
	const Vec3f& GetAccelerate() const { return m_Accelerate; }

	template<bool SwapBytesOrder>
	ParticleEmitterImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t emitterID;
		std::string emitterName;
		inputArchive >> emitterID >> emitterName;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
		inputArchive >> GetPosition() >> GetVelocity() >> GetAccelerate();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName()
			<< GetPosition() << GetVelocity() << GetAccelerate();
		return *this;
	}

private:
	ParticleEmitterID m_id;
	std::string m_name;

	cd::Vec3f m_Position;
	cd::Vec3f m_Velocity;
	cd::Vec3f m_Accelerate;
};

}