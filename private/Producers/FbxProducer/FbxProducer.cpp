#include "Producers/FbxProducer/FbxProducer.h"
#include "FbxProducerImpl.h"

namespace cdtools
{

FbxProducer::FbxProducer(const char* pFilePath)
{
	m_pFbxProducerImpl = new FbxProducerImpl(pFilePath);
}

FbxProducer::~FbxProducer()
{
	if (m_pFbxProducerImpl)
	{
		delete m_pFbxProducerImpl;
		m_pFbxProducerImpl = nullptr;
	}
}

void FbxProducer::Init()
{
	m_pFbxProducerImpl->Init();
}

void FbxProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pFbxProducerImpl->Execute(pSceneDatabase);
}

void FbxProducer::TraverseNode(fbxsdk::FbxNode* pSDKNode, cd::SceneDatabase* pSceneDatabase)
{
	m_pFbxProducerImpl->TraverseNode(pSDKNode, pSceneDatabase);
}

bool FbxProducer::TraverseMeshNode(fbxsdk::FbxNode* pMeshNode, cd::SceneDatabase* pSceneDatabase)
{
	return m_pFbxProducerImpl->TraverseMeshNode(pMeshNode, pSceneDatabase);
}

bool FbxProducer::TraverseTransformNode(fbxsdk::FbxNode* pTransformNode, cd::SceneDatabase* pSceneDatabase)
{
	return m_pFbxProducerImpl->TraverseTransformNode(pTransformNode, pSceneDatabase);
}

}