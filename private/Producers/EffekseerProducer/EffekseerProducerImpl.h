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

	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	const char16_t* m_pFilePath;
	Effekseer::random_vector3d m_particlePos;
	Effekseer::random_vector3d m_particleVelocity;
	Effekseer::random_vector3d m_particleAccelerate;

	cd::ObjectIDGenerator<cd::ParticleEmitterID> m_particleEmitterIDGenerator;
};

}