#pragma once

#include "Consumer/BaseConsumer.h"

namespace cdtools
{

class GenericConsumer final : public BaseConsumer
{
public:
	using BaseConsumer::BaseConsumer;
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;
};

}