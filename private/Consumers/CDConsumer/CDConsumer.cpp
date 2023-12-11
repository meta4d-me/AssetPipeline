#include "Consumers/CDConsumer/CDConsumer.h"
#include "CDConsumerImpl.h"

namespace cdtools
{

CDConsumer::CDConsumer(const char* pFilePath)
{
	m_pCDConsumerImpl = new CDConsumerImpl(pFilePath);
}

CDConsumer::~CDConsumer()
{
	if (m_pCDConsumerImpl)
	{
		delete m_pCDConsumerImpl;
		m_pCDConsumerImpl = nullptr;
	}
}

ExportMode CDConsumer::GetExportMode() const
{
	return m_pCDConsumerImpl->GetExportMode();
}

void CDConsumer::SetExportMode(ExportMode mode)
{
	m_pCDConsumerImpl->SetExportMode(mode);
}

cd::EndianType CDConsumer::GetTargetEndian() const
{
	return m_pCDConsumerImpl->GetTargetEndian();
}

void CDConsumer::SetTargetEndian(cd::EndianType endian)
{
	m_pCDConsumerImpl->SetTargetEndian(endian);
}

void CDConsumer::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCDConsumerImpl->Execute(pSceneDatabase);
}

void CDConsumer::ExportPureBinary(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCDConsumerImpl->ExportPureBinary(pSceneDatabase);
}

void CDConsumer::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCDConsumerImpl->ExportXmlBinary(pSceneDatabase);
}

void CDConsumer::EnableOption(CDConsumerOptions option)
{
	m_pCDConsumerImpl->GetOptions().Enable(option);
}

void CDConsumer::DisableOption(CDConsumerOptions option)
{
	m_pCDConsumerImpl->GetOptions().Disable(option);
}

bool CDConsumer::IsOptionEnabled(CDConsumerOptions option) const
{
	return m_pCDConsumerImpl->GetOptions().IsEnabled(option);
}

}