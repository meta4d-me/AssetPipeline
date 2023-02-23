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
	explicit ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase = nullptr);
	ProcessorImpl(const ProcessorImpl&) = delete;
	ProcessorImpl& operator=(const ProcessorImpl&) = delete;
	ProcessorImpl(ProcessorImpl&&) = delete;
	ProcessorImpl& operator=(ProcessorImpl&&) = delete;
	~ProcessorImpl();

	const cd::SceneDatabase* GetSceneDatabase() const { return m_pCurrentSceneDatabase; }
	void Run();

	void SetValidateSceneDatabaseEnable(bool enable) { m_enableValidateSceneDatabase = enable; }
	bool IsValidateSceneDatabaseEnabled() const { return m_enableValidateSceneDatabase; }

	void SetDumpSceneDatabaseEnable(bool enable) { m_enableDumpSceneDatabase = enable; }
	bool IsDumpSceneDatabaseEnabled() const { return m_enableDumpSceneDatabase; }

private:
	void DumpSceneDatabase();
	void ValidateSceneDatabase();

private:
	IProducer* m_pProducer = nullptr;
	IConsumer* m_pConsumer = nullptr;

	cd::SceneDatabase* m_pCurrentSceneDatabase;
	std::unique_ptr<cd::SceneDatabase> m_pLocalSceneDatabase;

	bool m_enableDumpSceneDatabase = true;
	bool m_enableValidateSceneDatabase = true;
};

}