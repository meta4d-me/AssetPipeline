#pragma once

#include "Base/Export.h"

#include <memory>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class IConsumer;
class IProducer;
class ProcessorImpl;

class TOOL_API Processor final
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
	ProcessorImpl* m_pProcessorImpl;
};

}