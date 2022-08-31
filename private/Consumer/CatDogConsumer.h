#pragma once

#include <string>

#include "Consumer/IConsumer.h"

namespace cdtools
{

class CatDogConsumer final : public IConsumer
{
public:
	CatDogConsumer() = delete;
	CatDogConsumer(std::string filePath);

	virtual void Execute(const SceneDatabase* pSceneDatabase) override;

private:
	std::string m_filePath;
};

}