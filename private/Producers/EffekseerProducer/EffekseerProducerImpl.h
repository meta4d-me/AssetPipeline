#pragma once

#include "Base/Template.h"
#include "Scene/ObjectIDGenerator.h"
#include "Scene/SceneDatabase.h"

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

	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	const char16_t* m_pFilePath;

	cd::ObjectIDGenerator<cd::ParticleEmitterID> m_particleEmitterIDGenerator;
};

}