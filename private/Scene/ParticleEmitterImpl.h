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

	void SetType(int Type) { m_type = MoveTemp(Type); }
	int& GetType() { return m_type; }
	const int& GetType() const { return m_type; }

	//Position
	void SetPosition(Vec3f position) { m_position = MoveTemp(position); }
	Vec3f& GetPosition() { return m_position; }
	const Vec3f& GetPosition() const { return m_position; }

	//Veloctity
	void SetVelocity(Vec3f velocity) { m_velocity = MoveTemp(velocity); }
	Vec3f& GetVelocity() { return m_velocity; }
	const Vec3f& GetVelocity() const { return m_velocity; }

	//Accelerate
	void SetAccelerate(Vec3f accelerate) { m_accelerate = MoveTemp(accelerate); }
	Vec3f& GetAccelerate() { return m_accelerate; }
	const Vec3f& GetAccelerate() const { return m_accelerate; }

	//Color
	void SetColor(Vec4f color) { m_color = MoveTemp(color); }
	Vec4f& GetColor() { return m_color; }
	const Vec4f& GetColor() const { return m_color; }

	template<bool SwapBytesOrder>
	ParticleEmitterImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t emitterID;
		std::string emitterName;
		inputArchive >> emitterID >> emitterName;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
		inputArchive >> GetType() >> GetPosition() >> GetVelocity() >> GetAccelerate()
			>> GetColor();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetType()
			<< GetPosition() << GetVelocity() << GetAccelerate()
			<< GetColor();
		return *this;
	}

private:
	ParticleEmitterID m_id;
	std::string m_name;

	int m_type;
	cd::Vec3f m_position;
	cd::Vec3f m_velocity;
	cd::Vec3f m_accelerate;
	cd::Vec4f m_color;
};

}