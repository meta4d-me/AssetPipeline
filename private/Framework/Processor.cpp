#include "Framework/Processor.h"
#include "ProcessorImpl.h"

namespace cdtools
{

Processor::Processor(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase)
{
	m_pProcessorImpl = new ProcessorImpl(pProducer, pConsumer, pHostSceneDatabase);
}

Processor::~Processor()
{
	if (m_pProcessorImpl)
	{
		delete m_pProcessorImpl;
		m_pProcessorImpl = nullptr;
	}
}

const cd::SceneDatabase* Processor::GetSceneDatabase() const
{
	return m_pProcessorImpl->GetSceneDatabase();
}

void Processor::Run()
{
	m_pProcessorImpl->Run();
}

}