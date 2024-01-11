#include "Producers/FbxProducer/FbxProducer.h"
#include "FbxProducerImpl.h"

namespace cdtools
{

FbxProducer::FbxProducer(const char* pFilePath)
{
	m_pFbxProducerImpl = new FbxProducerImpl(pFilePath);
}

FbxProducer::~FbxProducer()
{
	if (m_pFbxProducerImpl)
	{
		delete m_pFbxProducerImpl;
		m_pFbxProducerImpl = nullptr;
	}
}

void FbxProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pFbxProducerImpl->Execute(pSceneDatabase);
}

void FbxProducer::EnableOption(FbxProducerOptions option)
{
	m_pFbxProducerImpl->GetOptions().Enable(option);
}

void FbxProducer::DisableOption(FbxProducerOptions option)
{
	m_pFbxProducerImpl->GetOptions().Disable(option);
}

bool FbxProducer::IsOptionEnabled(FbxProducerOptions option) const
{
	return m_pFbxProducerImpl->GetOptions().IsEnabled(option);
}

}