#pragma once

#include "Base/Export.h"
#include "Framework/ProcessorOptions.h"

#include <memory>

namespace cd
{

class AxisSystem;
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

	void AddExtraTextureSearchFolder(const char* pFolderPath);
	bool IsSearchMissingTexturesEnabled() const;

	void SetAxisSystem(cd::AxisSystem axisSystem);
	const cd::SceneDatabase* GetSceneDatabase() const;
	void Run();

	void EnableOption(ProcessorOptions option);
	void DisableOption(ProcessorOptions option);
	bool IsOptionEnabled(ProcessorOptions option) const;

private:
	ProcessorImpl* m_pProcessorImpl;
};

}