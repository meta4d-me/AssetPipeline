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

class CORE_API Processor final
{
public:
	Processor() = delete;
	explicit Processor(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase = nullptr);
	Processor(const Processor&) = delete;
	Processor& operator=(const Processor&) = delete;
	Processor(Processor&&) = delete;
	Processor& operator=(Processor&&) = delete;
	~Processor();

	void SetValidateSceneDatabaseEnable(bool enable);
	bool IsValidateSceneDatabaseEnabled() const;

	void SetDumpSceneDatabaseEnable(bool enable);
	bool IsDumpSceneDatabaseEnabled() const;

	const cd::SceneDatabase* GetSceneDatabase() const;
	void Run();

private:
	ProcessorImpl* m_pProcessorImpl;
};

}