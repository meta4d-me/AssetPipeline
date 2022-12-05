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

class Processor final
{
public:
	Processor() = delete;
	explicit Processor(IProducer* pProducer, IConsumer* pConsumer);
	Processor(const Processor&) = delete;
	Processor& operator=(const Processor&) = delete;
	Processor(Processor&&) = delete;
	Processor& operator=(Processor&&) = delete;
	~Processor();

	void Run();

private:
	IProducer* m_pProducer = nullptr;
	IConsumer* m_pConsumer = nullptr;

	std::unique_ptr<cd::SceneDatabase> m_pSceneDatabase;
};

}