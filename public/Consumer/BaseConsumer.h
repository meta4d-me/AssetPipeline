#pragma once

#include "IConsumer.h"

#include <string>

namespace cdtools
{

class SceneDatabase;

class BaseConsumer : public IConsumer
{
public:
	BaseConsumer() = delete;
	explicit BaseConsumer(std::string filePath) : m_filePath(std::move(filePath)) {}
	BaseConsumer(const BaseConsumer&) = delete;
	BaseConsumer& operator=(const BaseConsumer&) = delete;
	BaseConsumer(BaseConsumer&&) = delete;
	BaseConsumer& operator=(BaseConsumer&&) = delete;
	virtual ~BaseConsumer() = default;

	virtual void Execute(const SceneDatabase* pSceneDatabase) {}

protected:
	std::string m_filePath;
};

}