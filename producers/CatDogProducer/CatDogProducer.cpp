#include "CatDogProducer.h"
#include "CatDogProducerImpl.h"

namespace cdtools
{

CatDogProducer::CatDogProducer(const char* pFilePath)
{
	m_pCatDogProducerImpl = new CatDogProducerImpl(pFilePath);
}

CatDogProducer::~CatDogProducer()
{
	if (m_pCatDogProducerImpl)
	{
		delete m_pCatDogProducerImpl;
		m_pCatDogProducerImpl = nullptr;
	}
}

void CatDogProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pCatDogProducerImpl->Execute(pSceneDatabase);
}

}