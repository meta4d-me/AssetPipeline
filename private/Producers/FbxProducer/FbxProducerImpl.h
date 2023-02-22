#pragma once

#include "Base/Template.h"

#include <cstdint>
#include <string>

namespace fbxsdk
{

class FbxManager;
class FbxNode;

}

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class FbxProducerImpl final
{
public:
	FbxProducerImpl() = delete;
	explicit FbxProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	FbxProducerImpl(const FbxProducerImpl&) = delete;
	FbxProducerImpl& operator=(const FbxProducerImpl&) = delete;
	FbxProducerImpl(FbxProducerImpl&&) = delete;
	FbxProducerImpl& operator=(FbxProducerImpl&&) = delete;
	~FbxProducerImpl();

	void Execute(cd::SceneDatabase* pSceneDatabase);

	void Init();

private:
	void TraverseNode(fbxsdk::FbxNode* pSDKNode, cd::SceneDatabase* pSceneDatabase);
	bool TraverseMeshNode(fbxsdk::FbxNode* pMeshNode, cd::SceneDatabase* pSceneDatabase);
	bool TraverseTransformNode(fbxsdk::FbxNode* pTransformNode, cd::SceneDatabase* pSceneDatabase);

private:
	uint32_t m_nodeIDCounter = 0U;
	fbxsdk::FbxManager* m_pSDKManager = nullptr;
	std::string m_filePath;
};

}