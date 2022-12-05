#pragma once

#include "IProducer.h"
#include "Base/Template.h"

#include <string>

namespace cdtools
{

class SceneDatabase;

class BaseProducer : public IProducer
{
public:
	BaseProducer() = delete;
	explicit BaseProducer(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	BaseProducer(const BaseProducer&) = delete;
	BaseProducer& operator=(const BaseProducer&) = delete;
	BaseProducer(BaseProducer&&) = delete;
	BaseProducer& operator=(BaseProducer&&) = delete;
	virtual ~BaseProducer() = default;

	virtual void Execute(const SceneDatabase* pSceneDatabase) {}

protected:
	std::string m_filePath;
};

}