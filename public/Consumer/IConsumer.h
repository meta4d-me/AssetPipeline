#pragma once

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class IConsumer
{
public:
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) = 0;
};

}