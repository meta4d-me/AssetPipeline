#pragma once

#include <string>

#include "Producer/IProducer.h"

namespace cdtools
{

class FbxProducer final : public IProducer
{
public:
	FbxProducer() = delete;
	FbxProducer(std::string filePath);

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	std::string m_filePath;
};

}