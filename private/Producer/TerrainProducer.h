#pragma once

#include <stdint.h>

#include "Producer/IProducer.h"

namespace cdtools
{

class TerrainProducer : public IProducer
{

public:
	explicit TerrainProducer(uint32_t x_vertices, uint32_t z_vertices, uint32_t width, uint32_t height, uint32_t max_elevation);
	TerrainProducer(const TerrainProducer& rhs) = default;
	TerrainProducer& operator=(const TerrainProducer& rhs) = default;
	TerrainProducer(TerrainProducer&& rhs) = default;
	TerrainProducer& operator=(TerrainProducer&& rhs) = default;
	virtual ~TerrainProducer() = default;

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	uint32_t m_numVerticesInX;	// vertices in x-axis
	uint32_t m_numVerticesInZ;	// vertices in z-axis
	uint32_t m_width;			// dist between x-axis vertices
	uint32_t m_height;			// dist between z-axis vertices
	uint32_t m_maxElevation;	// highest possible elevation; we start at 0.

};

}	// namespace cdtools