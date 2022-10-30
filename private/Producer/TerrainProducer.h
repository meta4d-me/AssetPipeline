#pragma once

#include "Producer/BaseProducer.h"

namespace cdtools
{

class TerrainProducer final : public BaseProducer
{
public:
	using BaseProducer::BaseProducer;
	virtual void Execute(SceneDatabase* pSceneDatabase) override;
};

}