#include "EffekseerProducerImpl.h"

#include <codecvt>
#include <Effekseer.h>
#include <Effekseer.Modules.h>

namespace cdtools
{

EffekseerProducerImpl::EffekseerProducerImpl(const char16_t* pFilePath) :
	m_pFilePath(pFilePath)
{
}

void EffekseerProducerImpl::PushAllColor(Effekseer::AllTypeColorParameter* AllColor)
{
	//Color
	if (AllColor->type ==AllColor->Fixed)
	{
		m_Color = AllColor->fixed.all;
	}
	else if (AllColor->type == AllColor->Easing)
	{

	}
}

void EffekseerProducerImpl::TraverseNodeRecursively(Effekseer::EffectNode* pNode)
{
	Effekseer::EffectNodeType nodeType = pNode->GetType();
	if (Effekseer::EffectNodeType::Sprite == nodeType)
	{
		//PVA
		auto* pSpriteNode = static_cast<Effekseer::EffectNodeSprite*>(pNode);
		m_particlePos = pSpriteNode->TranslationParam.TranslationPVA.location;
		m_particleVelocity = pSpriteNode->TranslationParam.TranslationPVA.velocity;
		m_particleAccelerate = pSpriteNode->TranslationParam.TranslationPVA.acceleration;
		PushAllColor(&pSpriteNode->SpriteAllColor);

		return;
	}
	else if (Effekseer::EffectNodeType::Ribbon == nodeType)
	{

	}
	else if (Effekseer::EffectNodeType::Track == nodeType)
	{

	}
	else if (Effekseer::EffectNodeType::Ring == nodeType)
	{

	}
	else if (Effekseer::EffectNodeType::Model == nodeType)
	{

	}


	int childNodeCount = pNode->GetChildrenCount();
	for (int childIndex = 0; childIndex < childNodeCount; ++childIndex)
	{
		TraverseNodeRecursively(pNode->GetChild(childIndex));
	}
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

	//pos velocity scale
	//may be  get all?????
   TraverseNodeRecursively(pEffectData->GetRoot());

	//all Set
	cd::ParticleEmitter particleEmitter(particleEmitterID, pParticleEmitterName);
	particleEmitter.SetPosition(cd::Vec3f(m_particlePos.max.x, m_particlePos.max.y, m_particlePos.max.z));
	particleEmitter.SetVelocity(cd::Vec3f(m_particleVelocity.max.x, m_particleVelocity.max.y, m_particleVelocity.max.z));
	particleEmitter.SetAccelerate(cd::Vec3f(m_particleAccelerate.max.x, m_particleAccelerate.max.y, m_particleAccelerate.max.z));
	particleEmitter.SetColor(cd::Vec4f(m_Color.R,m_Color.G,m_Color.B,m_Color.A));
	pSceneDatabase->AddParticleEmitter(cd::MoveTemp(particleEmitter));
	//// Mesh
	//auto a = pEffectData->GetName();

	//auto b = pEffectData->GetColorImageCount();
	//
	//auto c = pEffectData->GetColorImagePath(0);


}

}