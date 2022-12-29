#pragma once

#include "Base/Template.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CDProducerImpl final
{
public:
	CDProducerImpl() = delete;
	explicit CDProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	CDProducerImpl(const CDProducerImpl&) = delete;
	CDProducerImpl& operator=(const CDProducerImpl&) = delete;
	CDProducerImpl(CDProducerImpl&&) = delete;
	CDProducerImpl& operator=(CDProducerImpl&&) = delete;
	~CDProducerImpl() = default;
	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
};

}