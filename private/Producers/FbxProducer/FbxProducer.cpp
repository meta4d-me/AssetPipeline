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

void FbxProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pFbxProducerImpl->Execute(pSceneDatabase);
}

void FbxProducer::SetWantImportMaterial(bool flag)
{
	m_pFbxProducerImpl->SetWantImportMaterial(flag);
}

bool FbxProducer::WantImportMaterial() const
{
	return m_pFbxProducerImpl->WantImportMaterial();
}

void FbxProducer::SetWantImportTexture(bool flag)
{
	m_pFbxProducerImpl->SetWantImportTexture(flag);
}

bool FbxProducer::WantImportTexture() const
{
	return m_pFbxProducerImpl->WantImportTexture();
}

void FbxProducer::SetWantImportSkinMesh(bool flag)
{
	m_pFbxProducerImpl->SetWantImportSkinMesh(flag);
}

bool FbxProducer::WantImportSkinMesh() const
{
	return m_pFbxProducerImpl->WantImportSkinMesh();
}

void FbxProducer::SetWantImportAnimation(bool flag)
{
	m_pFbxProducerImpl->SetWantImportAnimation(flag);
}

bool FbxProducer::WantImportAnimation() const
{
	return m_pFbxProducerImpl->WantImportAnimation();
}

void FbxProducer::SetWantTriangulate(bool flag)
{
	m_pFbxProducerImpl->SetWantTriangulate(flag);
}

bool FbxProducer::IsTriangulateActive() const
{
	return m_pFbxProducerImpl->IsTriangulateActive();
}

}