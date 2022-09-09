#pragma once

#include <string>

#include "Producer/IProducer.h"

namespace fbxsdk
{

class FbxManager;
class FbxNode;

}

namespace cdtools
{

class Mesh;

class FbxProducer final : public IProducer
{
public:
	FbxProducer() = delete;
	FbxProducer(std::string filePath);
	~FbxProducer();

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	void TraverseNode(fbxsdk::FbxNode* pSDKNode, SceneDatabase* pSceneDatabase);
	bool TraverseMeshNode(fbxsdk::FbxNode* pMeshNode, SceneDatabase* pSceneDatabase);
	bool TraverseTransformNode(fbxsdk::FbxNode* pTransformNode, SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
	
	uint32_t m_nodeIDCounter = 0U;
	fbxsdk::FbxManager* m_pSDKManager = nullptr;
};

}