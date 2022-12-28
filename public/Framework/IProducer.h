#pragma once

#include "Base/Export.h"

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CORE_API IProducer
{
public:
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) = 0;
};

}