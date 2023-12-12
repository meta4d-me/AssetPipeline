#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"
#include "Scene/VertexFormat.h"

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

	void SetPosition(Vec3f position) { m_position = MoveTemp(position); }
	Vec3f& GetPosition() { return m_position; }
	const Vec3f& GetPosition() const { return m_position; }

	void SetVelocity(Vec3f velocity) { m_velocity = MoveTemp(velocity); }
	Vec3f& GetVelocity() { return m_velocity; }
	const Vec3f& GetVelocity() const { return m_velocity; }

	void SetAccelerate(Vec3f accelerate) { m_accelerate = MoveTemp(accelerate); }
	Vec3f& GetAccelerate() { return m_accelerate; }
	const Vec3f& GetAccelerate() const { return m_accelerate; }

	void SetColor(Vec4f color) { m_color = MoveTemp(color); }
	Vec4f& GetColor() { return m_color; }
	const Vec4f& GetColor() const { return m_color; }

	void SetFixedRotation(Vec3f rotation) { m_fixedRotation = MoveTemp(rotation); }
	Vec3f& GetFixedRotation() { return m_fixedRotation; }
	const Vec3f& GetFixedRotation() const { return m_fixedRotation; }

	void SetFixedScale(Vec3f scale) { m_fixedScale = MoveTemp(scale); }
	Vec3f& GetFixedScale() { return m_fixedScale; }
	const Vec3f& GetFixedScale() const { return m_fixedScale; }

	template<bool SwapBytesOrder>
	ParticleEmitterImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t emitterID;
		std::string emitterName;
		inputArchive >> emitterID >> emitterName;
		Init(ParticleEmitterID(emitterID), cd::MoveTemp(emitterName));
		inputArchive >> GetType() >> GetPosition() >> GetVelocity() >> GetAccelerate()
			>> GetColor() >> GetFixedRotation() >> GetFixedScale();
		return *this;
	}

	template<bool SwapBytesOrder>
	const ParticleEmitterImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetType()
			<< GetPosition() << GetVelocity() << GetAccelerate()
			<< GetColor() << GetFixedRotation() << GetFixedScale();
		return *this;
	}

private:
	ParticleEmitterID m_id;
	std::string m_name;

	cd::VertexFormat m_vertexFormat;
	int m_type;
	cd::Vec3f m_position;
	cd::Vec3f m_velocity;
	cd::Vec3f m_accelerate;
	cd::Vec4f m_color;
	cd::Vec3f m_fixedRotation;
	cd::Vec3f m_fixedScale;
};

}