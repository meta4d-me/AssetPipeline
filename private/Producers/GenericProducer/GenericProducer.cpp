#include "Producers/GenericProducer/GenericProducer.h"
#include "GenericProducerImpl.h"

namespace cdtools
{

GenericProducer::GenericProducer(const char* pFilePath)
{
	m_pGenericProducerImpl = new GenericProducerImpl(pFilePath);
}

GenericProducer::~GenericProducer()
{
	if (m_pGenericProducerImpl)
	{
		delete m_pGenericProducerImpl;
		m_pGenericProducerImpl = nullptr;
	}
}

void GenericProducer::SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID)
{
	m_pGenericProducerImpl->SetSceneDatabaseIDs(nodeID, meshID, materialID, textureID, lightID);
}

void GenericProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pGenericProducerImpl->Execute(pSceneDatabase);
}

void GenericProducer::ActivateBoundingBoxService()
{
	m_pGenericProducerImpl->ActivateBoundingBoxService();
}

bool GenericProducer::IsBoundingBoxServiceActive() const
{
	return m_pGenericProducerImpl->IsBoundingBoxServiceActive();
}

void GenericProducer::ActivateFlattenHierarchyService()
{
	m_pGenericProducerImpl->ActivateFlattenHierarchyService();
}

bool GenericProducer::IsFlattenHierarchyServiceActive() const
{
	return m_pGenericProducerImpl->IsFlattenHierarchyServiceActive();
}

void GenericProducer::ActivateTriangulateService()
{
	m_pGenericProducerImpl->ActivateTriangulateService();
}

bool GenericProducer::IsTriangulateServiceActive() const
{
	return m_pGenericProducerImpl->IsFlattenHierarchyServiceActive();
}

void GenericProducer::ActivateTangentsSpaceService()
{
	m_pGenericProducerImpl->ActivateTangentsSpaceService();
}

bool GenericProducer::IsTangentsSpaceServiceActive() const
{
	return m_pGenericProducerImpl->IsFlattenHierarchyServiceActive();
}

void GenericProducer::ActivateCleanUnusedService()
{
	m_pGenericProducerImpl->ActivateCleanUnusedService();
}

bool GenericProducer::IsCleanUnusedServiceActive() const
{
	return m_pGenericProducerImpl->IsFlattenHierarchyServiceActive();
}

void GenericProducer::ActivateSimpleAnimationService()
{
	m_pGenericProducerImpl->ActivateSimpleAnimationService();
}

bool GenericProducer::IsSimpleAnimationActive() const
{
	return m_pGenericProducerImpl->IsSimpleAnimationActive();
}

}