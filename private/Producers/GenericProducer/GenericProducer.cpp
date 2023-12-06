#include "Producers/GenericProducer/GenericProducer.h"

#include "GenericProducerImpl.h"

namespace cdtools
{

GenericProducer::GenericProducer(const char* pFilePath)
{
	m_pGenericProducerImpl = new GenericProducerImpl(pFilePath);
}

GenericProducer::~GenericProducer()
{
	if (m_pGenericProducerImpl)
	{
		delete m_pGenericProducerImpl;
		m_pGenericProducerImpl = nullptr;
	}
}

void GenericProducer::SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID)
{
	m_pGenericProducerImpl->SetSceneDatabaseIDs(nodeID, meshID, materialID, textureID, lightID);
}

void GenericProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pGenericProducerImpl->Execute(pSceneDatabase);
}

void GenericProducer::EnableOption(GenericProducerOptions option)
{
	m_pGenericProducerImpl->GetOptions().Enable(option);
}

void GenericProducer::DisableOption(GenericProducerOptions option)
{
	m_pGenericProducerImpl->GetOptions().Disable(option);
}

bool GenericProducer::IsOptionEnabled(GenericProducerOptions option) const
{
	return m_pGenericProducerImpl->GetOptions().IsEnabled(option);
}

}