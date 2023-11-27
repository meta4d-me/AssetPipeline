#pragma once

#include "Base/Template.h"
#include "Hashers/StringHash.hpp"
#include "Scene/ObjectIDGenerator.h"
#include "Scene/SceneDatabase.h"

#include "Effekseer/Effekseer.EffectNode.h"
#include "Effekseer/Effekseer.EffectImplemented.h"
#include "Effekseer/Effekseer.EffectNodeSprite.h"

namespace cdtools
{

class EffekseerProducerImpl final
{
public:
	EffekseerProducerImpl() = delete;
	explicit EffekseerProducerImpl(const char16_t* pFilePath);
	EffekseerProducerImpl(const EffekseerProducerImpl&) = delete;
	EffekseerProducerImpl& operator=(const EffekseerProducerImpl&) = delete;
	EffekseerProducerImpl(EffekseerProducerImpl&&) = delete;
	EffekseerProducerImpl& operator=(EffekseerProducerImpl&&) = delete;
	~EffekseerProducerImpl() = default;

	void TraverseNodeRecursively(Effekseer::EffectNode* pNode);

	void PushAllColor(Effekseer::AllTypeColorParameter* AllColor);

	void PushRotate(Effekseer::EffectNodeSprite* node);

	void PushScale(Effekseer::EffectNodeSprite* node);

	void JudgeRotationType(Effekseer::RotationParameter* Type);

	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	const char16_t* m_pFilePath;
	std::vector<Effekseer::EffectNodeType> m_particleType;
	std::vector<Effekseer::random_vector3d> m_particlePos;
	std::vector<Effekseer::random_vector3d> m_particleVelocity;
	std::vector<Effekseer::random_vector3d> m_particleAccelerate;
	std::vector<Effekseer::Color> m_fixedColor;
	std::vector<Effekseer::Vector3D> m_fixedRotation;
	std::vector<Effekseer::Vector3D> m_fixedScale;


	cd::ObjectIDGenerator<cd::ParticleEmitterID> m_particleEmitterIDGenerator;
};

}