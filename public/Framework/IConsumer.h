#pragma once

#include "Base/Export.h"

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class TOOL_API IConsumer
{
public:
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) = 0;
};

}