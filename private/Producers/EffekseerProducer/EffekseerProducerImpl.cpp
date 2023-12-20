#include "EffekseerProducerImpl.h"

#include "Producers/EffekseerProducer/EffekseerProducer.h"

#include <Effekseer.h>
#include <Effekseer.Modules.h>

#include <codecvt>

namespace cdtools
{

static std::map<Effekseer::EffectNodeType, cd::ParticleEmitterType> EffectNodeTypeMapping = {
	{ Effekseer::EffectNodeType::Root, cd::ParticleEmitterType::Root },
	{ Effekseer::EffectNodeType::NoneType, cd::ParticleEmitterType::None },
	{ Effekseer::EffectNodeType::Sprite, cd::ParticleEmitterType::Sprite },
	{ Effekseer::EffectNodeType::Ribbon, cd::ParticleEmitterType::Ribbon },
	{ Effekseer::EffectNodeType::Ring, cd::ParticleEmitterType::Ring },
	{ Effekseer::EffectNodeType::Model, cd::ParticleEmitterType::Model },
	{ Effekseer::EffectNodeType::Track, cd::ParticleEmitterType::Track },
};

EffekseerProducerImpl::EffekseerProducerImpl(const char16_t* pFilePath) :
	m_pFilePath(pFilePath)
{
}

void EffekseerProducerImpl::PushSpawnCount(Effekseer::ParameterCommonValues* pParameter)
{
	m_particleMaxCount.push_back(pParameter->MaxGeneration);
}

void EffekseerProducerImpl::PushPVA(Effekseer::ParameterTranslationPVA* pParameter)
{
	m_particlePos.push_back(pParameter->location);
	m_particleVelocity.push_back(pParameter->velocity);
	m_particleAccelerate.push_back(pParameter->acceleration);
}

void EffekseerProducerImpl::PushAllColor(Effekseer::AllTypeColorParameter* pAllColor)
{
	if (pAllColor->type == pAllColor->Fixed)
	{
		m_fixedColor.push_back(pAllColor->fixed.all);
	}
	else if (pAllColor->type == pAllColor->Random)
	{

	}
	else if (pAllColor->type == pAllColor->Easing)
	{

	}
	//TODO : there still have some num to analysis
}

void EffekseerProducerImpl::JudgeRotationType(Effekseer::RotationParameter* pParameter)
{
	if (pParameter->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_Fixed)
	{
		m_fixedRotation.push_back(pParameter->RotationFixed.Position);
	}
	else if (pParameter->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_PVA)
	{

	}
	else if (pParameter->RotationType == Effekseer::ParameterRotationType::ParameterRotationType_Easing)
	{

	}
	///TODO:  there still some num to analysis
}

void EffekseerProducerImpl::PushRotate(Effekseer::EffectNodeSprite* pNode)
{

	if (pNode->Billboard == Effekseer::BillboardType::Billboard)
	{
		//NO Rotation
		JudgeRotationType(&pNode->RotationParam);
	}
	else if (pNode->Billboard == Effekseer::BillboardType::RotatedBillboard)
	{
		//Z Rotation
		JudgeRotationType(&pNode->RotationParam);
	}
	else if (pNode->Billboard == Effekseer::BillboardType::YAxisFixed)
	{
		//Y Rotation
		JudgeRotationType(&pNode->RotationParam);
	}
	else if (pNode->Billboard == Effekseer::BillboardType::Fixed)
	{
		// XYZ Rotation
		JudgeRotationType(&pNode->RotationParam);
	}
}

void EffekseerProducerImpl::PushScale(Effekseer::EffectNodeSprite* pNode)
{
	if (pNode->ScalingParam.ScalingType == Effekseer::ParameterScalingType::ParameterScalingType_Fixed)
	{
		m_fixedScale.push_back(pNode->ScalingParam.ScalingFixed.Position);
	}
	else if (pNode->ScalingParam.ScalingType == Effekseer::ParameterScalingType::ParameterScalingType_PVA)
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
		PushSpawnCount(&pSpriteNode->CommonValues);
		PushPVA(&pSpriteNode->TranslationParam.TranslationPVA);
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

//cd::Mesh EffekseerProducerImpl::GenerateParticleMesh()
//{
//	//const std::string terrainMeshName = string_format("TerrainSector(%d, %d)", sector_x, sector_z);
//	//const MeshID::ValueType meshHash = StringHash<MeshID::ValueType>(terrainMeshName);
//	//const MeshID terrainMeshID = m_meshIDGenerator.AllocateID(meshHash);
//	//Mesh terrain(terrainMeshID, terrainMeshName.c_str(), m_verticesPerSector, m_trianglesPerSector);
//	
//	//TODO :  change particleType
//	const std::string particleMeshName = std::format("{}{}", "particleType", "mesh");
//	const cd::MeshID::ValueType meshHash = cd::StringHash<cd::MeshID::ValueType>(particleMeshName);
//	const cd::MeshID particleMeshID = m_particleMeshID.AllocateID(meshHash);
//	//TODO:  change (11 11)  to  (vertexCount   polygonCount)
//	cd::Mesh particleMesh(particleMeshID, particleMeshName.c_str(), 11, 11);
//	//particleMesh.SetVertexPosition();
//	//particleMesh.SetVertexColor();
//	//particleMesh.SetVertexUV();
//
//	return particleMesh;
//}


void EffekseerProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	auto efkManager = ::Effekseer::Manager::Create(8000);
	auto effect = Effekseer::Effect::Create(efkManager, m_pFilePath);
	auto* pEffectData = effect.Get();

	//ID name
	const char16_t* pEffectName = pEffectData->GetName();
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
	std::u16string u16str = pEffectName;
	std::string str = converter.to_bytes(u16str);
	const char* pAllParticleEmitterName = str.c_str();

	////Get Mesh From efkefc
	//cd::Mesh generatedMesh = GenerateParticleMesh();
	//pSceneDatabase->AddMesh(cd::MoveTemp(generatedMesh));

	// pos velocity scale
	// TODO : check other properties.
	TraverseNodeRecursively(pEffectData->GetRoot());
	for (int i = 0; i < pEffectData->GetRoot()->GetChildrenCount(); i++)
	{
		cd::ParticleEmitterID::ValueType particleEmitterHash = cd::StringHash<cd::ParticleEmitterID::ValueType>(pAllParticleEmitterName + i);
		cd::ParticleEmitterID particleEmitterID = m_particleEmitterIDGenerator.AllocateID(particleEmitterHash);

		cd::ParticleEmitter particleEmitter(particleEmitterID, std::format("{}{}", pAllParticleEmitterName, i).c_str());
		particleEmitter.SetType(EffectNodeTypeMapping[static_cast<Effekseer::EffectNodeType>(m_particleType[i])]);
		particleEmitter.SetMaxCount(m_particleMaxCount[i]);
		particleEmitter.SetPosition(MeanXYZ(m_particlePos[i].max, m_particlePos[i].min));
		particleEmitter.SetVelocity(MeanXYZ(m_particleVelocity[i].max, m_particleVelocity[i].min));
		particleEmitter.SetAccelerate(MeanXYZ(m_particleAccelerate[i].max, m_particleAccelerate[i].min));
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