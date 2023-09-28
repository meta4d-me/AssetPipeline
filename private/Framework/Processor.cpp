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

void Processor::SetCalculateAABBForSceneDatabaseEnable(bool enable)
{
	m_pProcessorImpl->SetCalculateAABBForSceneDatabaseEnable(enable);
}

bool Processor::IsCalculateAABBForSceneDatabaseEnabled() const
{
	return m_pProcessorImpl->IsCalculateAABBForSceneDatabaseEnabled();
}

void Processor::SetFlattenSceneDatabaseEnable(bool enable)
{
	m_pProcessorImpl->SetFlattenSceneDatabaseEnable(enable);
}

bool Processor::IsFlattenSceneDatabaseEnabled() const
{
	return m_pProcessorImpl->IsFlattenSceneDatabaseEnabled();
}

void Processor::AddExtraTextureSearchFolder(const char* pFolderPath)
{
	m_pProcessorImpl->AddExtraTextureSearchFolder(pFolderPath);
}

bool Processor::IsSearchMissingTexturesEnabled() const
{
	return m_pProcessorImpl->IsSearchMissingTexturesEnabled();
}

void Processor::SetEmbedTextureFilesEnable(bool enable)
{
	m_pProcessorImpl->SetEmbedTextureFilesEnable(enable);
}

bool Processor::IsEmbedTextureFilesEnabled() const
{
	return m_pProcessorImpl->IsEmbedTextureFilesEnabled();
}

}