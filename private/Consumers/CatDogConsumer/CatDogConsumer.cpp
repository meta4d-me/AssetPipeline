#include "Consumers/CatDogConsumer/CatDogConsumer.h"
#include "CatDogConsumerImpl.h"

namespace cdtools
{

CatDogConsumer::CatDogConsumer(const char* pFilePath)
{
	m_pCatDogConsumerImpl = new CatDogConsumerImpl(pFilePath);
}

CatDogConsumer::~CatDogConsumer()
{
	if (m_pCatDogConsumerImpl)
	{
		delete m_pCatDogConsumerImpl;
		m_pCatDogConsumerImpl = nullptr;
	}
}

ExportMode CatDogConsumer::GetExportMode() const
{
	return m_pCatDogConsumerImpl->GetExportMode();
}

void CatDogConsumer::SetExportMode(ExportMode mode)
{
	m_pCatDogConsumerImpl->SetExportMode(mode);
}

void CatDogConsumer::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCatDogConsumerImpl->Execute(pSceneDatabase);
}

void CatDogConsumer::ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, cd::endian targetEndian)
{
	m_pCatDogConsumerImpl->ExportPureBinary(pSceneDatabase, targetEndian);
}

void CatDogConsumer::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase)
{
	m_pCatDogConsumerImpl->ExportXmlBinary(pSceneDatabase);
}

}