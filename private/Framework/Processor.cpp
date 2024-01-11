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

void Processor::SetAxisSystem(cd::AxisSystem axisSystem)
{
	m_pProcessorImpl->SetAxisSystem(cd::MoveTemp(axisSystem));
}

void Processor::AddExtraTextureSearchFolder(const char* pFolderPath)
{
	m_pProcessorImpl->AddExtraTextureSearchFolder(pFolderPath);
}

void Processor::EnableOption(ProcessorOptions option)
{
	m_pProcessorImpl->GetOptions().Enable(option);
}

void Processor::DisableOption(ProcessorOptions option)
{
	m_pProcessorImpl->GetOptions().Disable(option);
}

bool Processor::IsOptionEnabled(ProcessorOptions option) const
{
	return m_pProcessorImpl->GetOptions().IsEnabled(option);
}

}