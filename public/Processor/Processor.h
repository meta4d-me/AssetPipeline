#pragma once

namespace cdtools
{

class IConsumer;
class IProducer;
class SceneDatabase;

class Processor final
{
public:
	Processor() = delete;
	Processor(IProducer* pProducer, IConsumer* pConsumer);
	~Processor();

	Processor(const Processor&) = delete;
	Processor& operator=(const Processor&) = delete;
	Processor(Processor&&) = delete;
	Processor& operator=(Processor&&) = delete;

	void Run();

private:
	IProducer* m_pProducer = nullptr;
	IConsumer* m_pConsumer = nullptr;

	SceneDatabase* m_pSceneDatabase = nullptr;
};

}