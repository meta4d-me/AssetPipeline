#include "Producers/TerrainProducer/TerrainProducer.h"
#include "TerrainProducerImpl.h"

#include "Scene/Mesh.h"

namespace cdtools
{

TerrainProducer::TerrainProducer(const TerrainGenParams& genParams)
{
	m_pTerrainProducerImpl = new TerrainProducerImpl(genParams);
}

TerrainProducer::~TerrainProducer()
{
	if (m_pTerrainProducerImpl)
	{
		delete m_pTerrainProducerImpl;
		m_pTerrainProducerImpl = nullptr;
	}
}

void TerrainProducer::Execute(cd::SceneDatabase* pSceneDatabase)
{
	m_pTerrainProducerImpl->Execute(pSceneDatabase);
}

cd::Mesh TerrainProducer::CreateTerrainMesh(uint32_t sector_x, uint32_t sector_z)
{
	return m_pTerrainProducerImpl->CreateTerrainMesh(sector_x, sector_z);
}

TerrainQuad TerrainProducer::CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const
{
	return m_pTerrainProducerImpl->CreateQuadAt(currentVertexId, currentPolygonId);
}

}	// namespace cdtools
