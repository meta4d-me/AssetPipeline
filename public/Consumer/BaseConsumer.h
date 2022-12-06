#pragma once

#include "IConsumer.h"
#include "Base/Template.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class BaseConsumer : public IConsumer
{
public:
	BaseConsumer() = delete;
	explicit BaseConsumer(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	BaseConsumer(const BaseConsumer&) = delete;
	BaseConsumer& operator=(const BaseConsumer&) = delete;
	BaseConsumer(BaseConsumer&&) = delete;
	BaseConsumer& operator=(BaseConsumer&&) = delete;
	virtual ~BaseConsumer() = default;

	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) {}

protected:
	std::string m_filePath;
};

}