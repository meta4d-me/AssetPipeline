#pragma once

namespace cdtools
{

class SceneDatabase;

class IProducer
{
public:
	virtual void Execute(SceneDatabase* pSceneDatabase) = 0;
};

}