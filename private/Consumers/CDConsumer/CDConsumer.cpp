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

void CDConsumer::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCDConsumerImpl->Execute(pSceneDatabase);
}

void CDConsumer::ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, uint8_t targetEndian)
{
	m_pCDConsumerImpl->ExportPureBinary(pSceneDatabase, targetEndian);
}

void CDConsumer::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCDConsumerImpl->ExportXmlBinary(pSceneDatabase);
}

}