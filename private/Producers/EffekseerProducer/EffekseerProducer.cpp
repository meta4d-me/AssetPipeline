#include "Producers/EffekseerProducer/EffekseerProducer.h"
#include "EffekseerProducerImpl.h"

namespace cdtools
{

EffekseerProducer::EffekseerProducer(const char* pFilePath)
{
	m_pEffekseerProducerImpl = new EffekseerProducerImpl(pFilePath);
}

EffekseerProducer::~EffekseerProducer()
{
	if (m_pEffekseerProducerImpl)
	{
		delete m_pEffekseerProducerImpl;
		m_pEffekseerProducerImpl = nullptr;
	}
}

void EffekseerProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pEffekseerProducerImpl->Execute(pSceneDatabase);
}

}