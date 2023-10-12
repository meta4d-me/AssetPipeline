#include "EffekseerProducerImpl.h"

#include <Effekseer.h>
#include <Effekseer.Modules.h>

namespace cdtools
{

EffekseerProducerImpl::EffekseerProducerImpl(const char16_t* pFilePath) :
	m_pFilePath(pFilePath)
{
}

void EffekseerProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	auto efkManager = ::Effekseer::Manager::Create(8000);
	auto effect = Effekseer::Effect::Create(efkManager, m_pFilePath);
	auto* pEffectData = effect.Get();

	// Mesh
	pEffectData->GetProceduralModelCount();
	pEffectData->GetWaveCount();

	// Materials and ParticleEmitter
	pEffectData->GetMaterialCount();
	pEffectData->GetLoadingParameter();
	pEffectData->GetDefaultDynamicInputs();
	
	// Texture
	pEffectData->GetColorImageCount();
	pEffectData->GetDistortionImageCount();

	// Track
	pEffectData->GetCurveCount();
}

}