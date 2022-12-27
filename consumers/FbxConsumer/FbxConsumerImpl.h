#pragma once

#include "Base/Template.h"

#include <string>

namespace fbxsdk
{

class FbxExporter;
class FbxManager;
class FbxScene;

}

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class FbxConsumerImpl final
{
public:
	FbxConsumerImpl() = delete;
	explicit FbxConsumerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	FbxConsumerImpl(const FbxConsumerImpl&) = delete;
	FbxConsumerImpl& operator=(const FbxConsumerImpl&) = delete;
	FbxConsumerImpl(FbxConsumerImpl&&) = delete;
	FbxConsumerImpl& operator=(FbxConsumerImpl&&) = delete;
	~FbxConsumerImpl();
	void Execute(const cd::SceneDatabase* pSceneDatabase);

private:
	fbxsdk::FbxManager*		m_pSDKManager = nullptr;
	fbxsdk::FbxScene*		m_pSDKScene = nullptr;
	fbxsdk::FbxExporter*	m_pSDKExporter = nullptr;
	std::string m_filePath;
};

}