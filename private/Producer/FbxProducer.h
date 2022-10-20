#pragma once

#include "Producer/BaseProducer.h"

namespace fbxsdk
{

class FbxManager;
class FbxNode;

}

namespace cdtools
{

class Mesh;

class FbxProducer final : public BaseProducer
{
public:
	using BaseProducer::BaseProducer;
	virtual ~FbxProducer();

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	void Init();
	void TraverseNode(fbxsdk::FbxNode* pSDKNode, SceneDatabase* pSceneDatabase);
	bool TraverseMeshNode(fbxsdk::FbxNode* pMeshNode, SceneDatabase* pSceneDatabase);
	bool TraverseTransformNode(fbxsdk::FbxNode* pTransformNode, SceneDatabase* pSceneDatabase);

private:
	uint32_t m_nodeIDCounter = 0U;
	fbxsdk::FbxManager* m_pSDKManager = nullptr;
};

}