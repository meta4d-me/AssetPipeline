#pragma once

#include "Framework/IProducer.h"

namespace cdtools
{

enum particleType
{
	Root = -1,
	NoneType = 0,
	Sprite = 2,
	Ribbon = 3,
	Ring = 4,
	Model = 5,
	Track = 6,
};

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