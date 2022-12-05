#pragma once

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class IProducer
{
public:
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) = 0;
};

}