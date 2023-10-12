#pragma once

#include "Framework/IProducer.h"

namespace cdtools
{

class EffekseerProducerImpl;

class TOOL_API EffekseerProducer final : public IProducer
{
public:
	EffekseerProducer() = delete;
	explicit EffekseerProducer(const char16_t* pFilePath);
	EffekseerProducer(const EffekseerProducer&) = delete;
	EffekseerProducer& operator=(const EffekseerProducer&) = delete;
	EffekseerProducer(EffekseerProducer&&) = delete;
	EffekseerProducer& operator=(EffekseerProducer&&) = delete;
	virtual ~EffekseerProducer();

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	EffekseerProducerImpl* m_pEffekseerProducerImpl;
};

}