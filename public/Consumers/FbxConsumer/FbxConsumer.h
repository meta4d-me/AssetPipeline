#pragma once

#include "Consumers/FbxConsumer/FbxConsumerOptions.h"
#include "Framework/IConsumer.h"

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class FbxConsumerImpl;

class TOOL_API FbxConsumer final : public IConsumer
{
public:
	FbxConsumer() = delete;
	explicit FbxConsumer(const char* pFilePath);
	FbxConsumer(const FbxConsumer&) = delete;
	FbxConsumer& operator=(const FbxConsumer&) = delete;
	FbxConsumer(FbxConsumer&&) = delete;
	FbxConsumer& operator=(FbxConsumer&&) = delete;
	virtual ~FbxConsumer();
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;

	void EnableOption(FbxConsumerOptions option);
	void DisableOption(FbxConsumerOptions option);
	bool IsOptionEnabled(FbxConsumerOptions option) const;

private:
	FbxConsumerImpl* m_pFbxConsumerImpl;
};

}