#pragma once

#include <stdint.h>

#include "Producer/IProducer.h"

namespace cdtools
{

class TerrainProducer : public IProducer
{

public:
	explicit TerrainProducer(uint32_t width, uint32_t height);
	TerrainProducer(const TerrainProducer& rhs) = default;
	TerrainProducer& operator=(const TerrainProducer& rhs) = default;
	TerrainProducer(TerrainProducer&& rhs) = default;
	TerrainProducer& operator=(TerrainProducer&& rhs) = default;
	virtual ~TerrainProducer() = default;

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	uint32_t m_width;	// vertices in x-axis
	uint32_t m_height;	// vertices in z-axis

};

}	// namespace cdtools