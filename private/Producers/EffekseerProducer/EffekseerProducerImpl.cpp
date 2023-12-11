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
		m_fixedColor .push_back(AllColor->fixed.all);
	}
	else if (AllColor->type == AllColor->Random)
	{

	}
	else if (AllColor->type == AllColor->Easing)
	{

	}
	//TODO : there still have some num to analysis
}

void EffekseerProducerImpl::JudgeRotationType(Effekseer::RotationParameter* Type)
{
	if (Type->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_Fixed)
	{
		m_fixedRotation.push_back(Type->RotationFixed.Position);
	}
	else if (Type->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_PVA)
	{

	}
	else if (Type->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_Easing)
	{

	}
	///TODO:  there still some num to analysis
}

void EffekseerProducerImpl::PushRotate(Effekseer::EffectNodeSprite* node)
{

	if (node->Billboard == Effekseer::BillboardType::Billboard)
	{
		//NO Rotation
		JudgeRotationType(&node->RotationParam);
	}
	else if (node->Billboard == Effekseer::BillboardType::RotatedBillboard)
	{
		//Z Rotation
		JudgeRotationType(&node->RotationParam);
	}
	else if (node->Billboard == Effekseer::BillboardType::YAxisFixed)
	{
		//Y Rotation
		JudgeRotationType(&node->RotationParam);
	}
	else if (node->Billboard == Effekseer::BillboardType::Fixed)
	{
		// XYZ Rotation
		JudgeRotationType(&node->RotationParam);
	}
}

void EffekseerProducerImpl::PushScale(Effekseer::EffectNodeSprite* node)
{
	if (node->ScalingParam.ScalingType == Effekseer::ParameterScalingType::ParameterScalingType_Fixed)
	{
		m_fixedScale.push_back(node->ScalingParam.ScalingFixed.Position);
	}
	else if (node->ScalingParam.ScalingType == Effekseer::ParameterScalingType::ParameterScalingType_PVA)
	{

	}
	///TODO:  there still some num to analysis
}

void EffekseerProducerImpl::TraverseNodeRecursively(Effekseer::EffectNode* pNode)
{
	Effekseer::EffectNodeType nodeType = pNode->GetType();
	if (Effekseer::EffectNodeType::Sprite == nodeType)
	{
		//PVA
		m_particleType.push_back(nodeType);
		auto* pSpriteNode = static_cast<Effekseer::EffectNodeSprite*>(pNode);
		m_particlePos.push_back(pSpriteNode->TranslationParam.TranslationPVA.location);
		m_particleVelocity.push_back(pSpriteNode->TranslationParam.TranslationPVA.velocity);
		m_particleAccelerate.push_back(pSpriteNode->TranslationParam.TranslationPVA.acceleration);
		PushAllColor(&pSpriteNode->SpriteAllColor);
		PushRotate(pSpriteNode);
		PushScale(pSpriteNode);
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

	//pos velocity scale
	//may be  get all?????
	TraverseNodeRecursively(pEffectData->GetRoot());
	for (int i = 0; i < pEffectData->GetRoot()->GetChildrenCount(); i++)
	{
		cd::ParticleEmitterID::ValueType particleEmitterHash = cd::StringHash<cd::ParticleEmitterID::ValueType>(pParticleEmitterName+i);
		cd::ParticleEmitterID particleEmitterID = m_particleEmitterIDGenerator.AllocateID(particleEmitterHash);
		//all Set
		cd::ParticleEmitter particleEmitter(particleEmitterID, pParticleEmitterName);
		particleEmitter.SetType(static_cast<int>(m_particleType[i]));
		if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Root)) { particleEmitter.SetTypeName("Root"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::NoneType)) { particleEmitter.SetTypeName("NoneType"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Sprite)) { particleEmitter.SetTypeName("Sprite"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Ribbon)) { particleEmitter.SetTypeName("Ribbon"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Ring)) { particleEmitter.SetTypeName("Ring"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Model)) { particleEmitter.SetTypeName("Model"); }
		else if (particleEmitter.GetType() == static_cast<int>(Effekseer::EffectNodeType::Track)) { particleEmitter.SetTypeName("Track"); }
		particleEmitter.SetPosition(cd::Vec3f(m_particlePos[i].max.x, m_particlePos[i].max.y, m_particlePos[i].max.z));
		particleEmitter.SetVelocity(cd::Vec3f(m_particleVelocity[i].max.x, m_particleVelocity[i].max.y, m_particleVelocity[i].max.z));
		particleEmitter.SetAccelerate(cd::Vec3f(m_particleAccelerate[i].max.x, m_particleAccelerate[i].max.y, m_particleAccelerate[i].max.z));
		particleEmitter.SetColor(cd::Vec4f(m_fixedColor[i].R, m_fixedColor[i].G, m_fixedColor[i].B, m_fixedColor[i].A));
		particleEmitter.SetFixedRotation(cd::Vec3f(m_fixedRotation[i].X, m_fixedRotation[i].Y, m_fixedRotation[i].Z));
		particleEmitter.SetFixedScale(cd::Vec3f(m_fixedScale[i].X, m_fixedScale[i].Y, m_fixedScale[i].Z));

		pSceneDatabase->AddParticleEmitter(cd::MoveTemp(particleEmitter));
	}
	//// Mesh
	//auto a = pEffectData->GetName();

	//auto b = pEffectData->GetColorImageCount();
	//
	//auto c = pEffectData->GetColorImagePath(0);


}

}