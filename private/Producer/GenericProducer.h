#pragma once

#include <string>

#include "Producer/IProducer.h"

namespace cdtools
{

// GenericProducer can be used to import all kinds of model formats, such as fbx, glTF, obj. Powered by assimp.
class GenericProducer final : public IProducer
{
public:
	GenericProducer() = delete;
	GenericProducer(std::string filePath);

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	std::string m_filePath;
};

}