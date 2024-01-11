#pragma once

#include "Framework/IProducer.h"
#include "Producers/FbxProducer/FbxProducerOptions.h"

namespace cdtools
{

class FbxProducerImpl;

class TOOL_API FbxProducer final : public IProducer
{
public:
	FbxProducer() = delete;
	explicit FbxProducer(const char* pFilePath);
	FbxProducer(const FbxProducer&) = delete;
	FbxProducer& operator=(const FbxProducer&) = delete;
	FbxProducer(FbxProducer&&) = delete;
	FbxProducer& operator=(FbxProducer&&) = delete;
	virtual ~FbxProducer();

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	void EnableOption(FbxProducerOptions option);
	void DisableOption(FbxProducerOptions option);
	bool IsOptionEnabled(FbxProducerOptions option) const;

private:
	FbxProducerImpl* m_pFbxProducerImpl;
};

}