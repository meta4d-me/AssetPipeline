#pragma once

#include <string>

#include "Producer/IProducer.h"

namespace cdtools
{

class CatDogProducer final : public IProducer
{
public:
	CatDogProducer() = delete;
	CatDogProducer(std::string filePath);

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

private:
	std::string m_filePath;
};

}