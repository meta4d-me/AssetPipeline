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

	void SetPosition(Vec3f position) { m_Position = MoveTemp(position); }
	Vec3f& GetPosition() { return m_Position; }
	const Vec3f& GetPosition() const { return m_Position; }

	template<bool SwapBytesOrder>
	ParticleEmitterImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t emitterID;
		std::string emitterName;
		inputArchive >> emitterID >> emitterName;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
	/*	inputArchive >> GetEye() >> GetLookAt() >> GetUp()
			>> GetNearPlane() >> GetFarPlane() >> GetAspect() >> GetFov();*/
		inputArchive >> GetPosition();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetPosition();
		return *this;
	}

private:
	ParticleEmitterID m_id;
	std::string m_name;

	cd::Vec3f m_Position;
};

}