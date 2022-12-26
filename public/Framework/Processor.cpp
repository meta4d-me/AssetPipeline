#include "Processor.h"
#include "ProcessorImpl.h"

namespace cdtools
{

Processor::Processor(IProducer* pProducer, IConsumer* pConsumer)
{
	m_pProcessorImpl = new ProcessorImpl(pProducer, pConsumer);
}

Processor::~Processor()
{
	if (m_pProcessorImpl)
	{
		delete m_pProcessorImpl;
		m_pProcessorImpl = nullptr;
	}
}

void Processor::Run()
{
	m_pProcessorImpl->Run();
}

}