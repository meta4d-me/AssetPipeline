#pragma once

#include <memory>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class IConsumer;
class IProducer;

class ProcessorImpl final
{
public:
	ProcessorImpl() = delete;
	explicit ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer);
	ProcessorImpl(const ProcessorImpl&) = delete;
	ProcessorImpl& operator=(const ProcessorImpl&) = delete;
	ProcessorImpl(ProcessorImpl&&) = delete;
	ProcessorImpl& operator=(ProcessorImpl&&) = delete;
	~ProcessorImpl();

	void Run();

private:
	IProducer* m_pProducer = nullptr;
	IConsumer* m_pConsumer = nullptr;

	std::unique_ptr<cd::SceneDatabase> m_pSceneDatabase;
};

}