#pragma once

#include "Framework/IProducer.h"

namespace cdtools
{

class CatDogProducerImpl;

class TOOL_API CatDogProducer final : public IProducer
{
public:
	CatDogProducer() = delete;
	explicit CatDogProducer(const char* pFilePath);
	CatDogProducer(const CatDogProducer&) = delete;
	CatDogProducer& operator=(const CatDogProducer&) = delete;
	CatDogProducer(CatDogProducer&&) = delete;
	CatDogProducer& operator=(CatDogProducer&&) = delete;
	virtual ~CatDogProducer();
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	CatDogProducerImpl* m_pCatDogProducerImpl;
};

}