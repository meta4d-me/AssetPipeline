#pragma once

namespace cdtools
{

class SceneDatabase;

class IConsumer
{
public:
	virtual void Execute(const SceneDatabase* pSceneDatabase) = 0;
};

}