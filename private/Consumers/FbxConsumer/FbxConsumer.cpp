#include "Consumers/FbxConsumer/FbxConsumer.h"
#include "FbxConsumerImpl.h"

namespace cdtools
{

FbxConsumer::FbxConsumer(const char* pFilePath)
{
	m_pFbxConsumerImpl = new FbxConsumerImpl(pFilePath);
}

FbxConsumer::~FbxConsumer()
{
	if (m_pFbxConsumerImpl)
	{
		delete m_pFbxConsumerImpl;
		m_pFbxConsumerImpl = nullptr;
	}
}

void FbxConsumer::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	m_pFbxConsumerImpl->Execute(pSceneDatabase);
}

void FbxConsumer::EnableOption(FbxConsumerOptions option)
{
	m_pFbxConsumerImpl->GetOptions().Enable(option);
}

void FbxConsumer::DisableOption(FbxConsumerOptions option)
{
	m_pFbxConsumerImpl->GetOptions().Disable(option);
}

bool FbxConsumer::IsOptionEnabled(FbxConsumerOptions option) const
{
	return m_pFbxConsumerImpl->GetOptions().IsEnabled(option);
}

}