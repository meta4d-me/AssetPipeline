#pragma once

#include "Consumer/BaseConsumer.h"

namespace fbxsdk
{

class FbxExporter;
class FbxManager;
class FbxScene;

}

namespace cdtools
{

class FbxConsumer final : public BaseConsumer
{
public:
	using BaseConsumer::BaseConsumer;
	virtual void Execute(const SceneDatabase* pSceneDatabase) override;
	virtual ~FbxConsumer();

private:
	fbxsdk::FbxManager*		m_pSDKManager = nullptr;
	fbxsdk::FbxScene*		m_pSDKScene = nullptr;
	fbxsdk::FbxExporter*	m_pSDKExporter = nullptr;
};

}