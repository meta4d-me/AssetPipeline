#pragma once

#include "Framework/IProducer.h"
#include "TerrainParameter.h"

namespace cd
{

class Mesh;

}

namespace cdtools
{

class TerrainProducerImpl;

class TOOL_API TerrainProducer : public IProducer
{
public:
	TerrainProducer() = delete;
	explicit TerrainProducer(const TerrainGenParams& genParams);
	TerrainProducer(const TerrainProducer& rhs) = delete;
	TerrainProducer& operator=(const TerrainProducer& rhs) = delete;
	TerrainProducer(TerrainProducer&& rhs) = delete;
	TerrainProducer& operator=(TerrainProducer&& rhs) = delete;
	virtual ~TerrainProducer();

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

private:
	cd::Mesh CreateTerrainMesh(uint32_t sector_x, uint32_t sector_z);
	TerrainQuad CreateQuadAt(uint32_t& currentVertexId, uint32_t& currentPolygonId) const;

private:
	TerrainProducerImpl* m_pTerrainProducerImpl;
};

}	// namespace cdtools
