#pragma once

#include "Framework/IProducer.h"
#include "Producers/CDProducer/CDProducerOptions.h"

namespace cdtools
{

class CDProducerImpl;

class TOOL_API CDProducer final : public IProducer
{
public:
	CDProducer() = delete;
	explicit CDProducer(const char* pFilePath);
	CDProducer(const CDProducer&) = delete;
	CDProducer& operator=(const CDProducer&) = delete;
	CDProducer(CDProducer&&) = delete;
	CDProducer& operator=(CDProducer&&) = delete;
	virtual ~CDProducer();
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	void EnableOption(CDProducerOptions option);
	void DisableOption(CDProducerOptions option);
	bool IsOptionEnabled(CDProducerOptions option) const;

private:
	CDProducerImpl* m_pCDProducerImpl;
};

}