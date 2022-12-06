#pragma once

#include "Producer/BaseProducer.h"

namespace cdtools
{

class CatDogProducer final : public BaseProducer
{
public:
	using BaseProducer::BaseProducer;
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;
};

}