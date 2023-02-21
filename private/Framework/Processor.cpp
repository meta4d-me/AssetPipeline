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

void Processor::SetValidateSceneDatabaseEnable(bool enable)
{
	m_pProcessorImpl->SetValidateSceneDatabaseEnable(enable);
}

bool Processor::IsValidateSceneDatabaseEnabled() const
{
	return m_pProcessorImpl->IsValidateSceneDatabaseEnabled();
}

void Processor::SetDumpSceneDatabaseEnable(bool enable)
{
	m_pProcessorImpl->SetDumpSceneDatabaseEnable(enable);
}

bool Processor::IsDumpSceneDatabaseEnabled() const
{
	return m_pProcessorImpl->IsDumpSceneDatabaseEnabled();
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