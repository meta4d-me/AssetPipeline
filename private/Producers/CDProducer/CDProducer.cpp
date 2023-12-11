#include "Producers/CDProducer/CDProducer.h"
#include "CDProducerImpl.h"

namespace cdtools
{

CDProducer::CDProducer(const char* pFilePath)
{
	m_pCDProducerImpl = new CDProducerImpl(pFilePath);
}

CDProducer::~CDProducer()
{
	if (m_pCDProducerImpl)
	{
		delete m_pCDProducerImpl;
		m_pCDProducerImpl = nullptr;
	}
}

void CDProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pCDProducerImpl->Execute(pSceneDatabase);
}

void CDProducer::EnableOption(CDProducerOptions option)
{
	m_pCDProducerImpl->GetOptions().Enable(option);
}

void CDProducer::DisableOption(CDProducerOptions option)
{
	m_pCDProducerImpl->GetOptions().Disable(option);
}

bool CDProducer::IsOptionEnabled(CDProducerOptions option) const
{
	return m_pCDProducerImpl->GetOptions().IsEnabled(option);
}

}