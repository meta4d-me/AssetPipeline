#pragma once

#include <string>

#include "Consumer/IConsumer.h"

namespace fbxsdk
{

class FbxExporter;
class FbxManager;
class FbxScene;

}

namespace cdtools
{

class FbxConsumer final : public IConsumer
{
public:
	FbxConsumer() = delete;
	FbxConsumer(std::string filePath);
	~FbxConsumer();

	virtual void Execute(const SceneDatabase* pSceneDatabase) override;

private:
	std::string m_filePath;

	fbxsdk::FbxManager*		m_pSDKManager = nullptr;
	fbxsdk::FbxScene*		m_pSDKScene = nullptr;
	fbxsdk::FbxExporter*	m_pSDKExporter = nullptr;
};

}