#include "EffekseerProducerImpl.h"
#include "Hashers/StringHash.hpp"

#include <codecvt>
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


	//ID name
	const char16_t* EffectName = pEffectData->GetName();
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	std::u16string u16str = EffectName;
	std::string str = converter.to_bytes(u16str);
	const char* pParticleEmitterName = str.c_str();
	cd::ParticleEmitterID::ValueType particleEmitterHash = cd::StringHash<cd::ParticleEmitterID::ValueType>(pParticleEmitterName);
	cd::ParticleEmitterID particleEmitterID = m_particleEmitterIDGenerator.AllocateID(particleEmitterHash);
	
	//pos
	Effekseer::Handle efkHandle = 0;
	efkHandle = efkManager->Play(effect, 0,0,0);
	efkManager->AddLocation(efkHandle,::Effekseer::Vector3D(1.0f,0.0f,0.0f));
	auto pos = efkManager->GetLocation(efkHandle);

	//all Set
	cd::ParticleEmitter particleEmitter(particleEmitterID, pParticleEmitterName);
	particleEmitter.SetPosition(cd::Vec3f(pos.X,pos.Y,pos.Z));

	pSceneDatabase->AddParticleEmitter(cd::MoveTemp(particleEmitter));
	//// Mesh
	//auto a = pEffectData->GetName();

	//auto b = pEffectData->GetColorImageCount();
	//
	//auto c = pEffectData->GetColorImagePath(0);


}

}