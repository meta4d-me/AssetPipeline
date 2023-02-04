#include "Producers/TerrainProducer/TerrainProducer.h"
#include "TerrainProducerImpl.h"

#include "Scene/Mesh.h"
#include "TerrainTypes.h"

namespace cdtools
{

TerrainProducer::TerrainProducer(const TerrainMetadata& terrainMetadata, const TerrainSectorMetadata& sectorMetadata)
{
	m_pTerrainProducerImpl = new TerrainProducerImpl(terrainMetadata, sectorMetadata);
}

TerrainProducer::~TerrainProducer()
{
	if (m_pTerrainProducerImpl)
	{
		delete m_pTerrainProducerImpl;
		m_pTerrainProducerImpl = nullptr;
	}
}

void TerrainProducer::SetSceneDatabaseIDs(const cd::SceneDatabase* pSceneDatabase)
{
	m_pTerrainProducerImpl->SetSceneDatabaseIDs(pSceneDatabase);
}

void TerrainProducer::SetTerrainMetadata(const TerrainMetadata& metadata)
{
	m_pTerrainProducerImpl->SetTerrainMetadata(metadata);
}

void TerrainProducer::SetSectorMetadata(const TerrainSectorMetadata& metadata)
{
	m_pTerrainProducerImpl->SetSectorMetadata(metadata);
}

void TerrainProducer::Initialize()
{
	m_pTerrainProducerImpl->Initialize();
}

uint32_t TerrainProducer::GetTerrainLengthInX() const
{
	return m_pTerrainProducerImpl->GetTerrainLengthInX();
}

uint32_t TerrainProducer::GetTerrainLengthInZ() const
{
	return m_pTerrainProducerImpl->GetTerrainLengthInZ();
}

uint32_t TerrainProducer::GetSectorCount() const
{
	return m_pTerrainProducerImpl->GetSectorCount();
}

uint32_t TerrainProducer::GetSectorLengthInX() const
{
	return m_pTerrainProducerImpl->GetSectorLengthInX();
}

uint32_t TerrainProducer::GetSectorLengthInZ() const
{
	return m_pTerrainProducerImpl->GetSectorLengthInZ();
}

uint32_t TerrainProducer::GetQuadsPerSector() const
{
	return m_pTerrainProducerImpl->GetQuadsPerSector();
}

uint32_t TerrainProducer::GetVertsPerSector() const
{
	return m_pTerrainProducerImpl->GetVertsPerSector();
}

void TerrainProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pTerrainProducerImpl->Execute(pSceneDatabase);
}

}	// namespace cdtools
