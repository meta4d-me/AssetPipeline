#pragma once

#include "Base/BitFlags.h"
#include "Base/Template.h"
#include "Framework/ProcessorOptions.h"
#include "Math/AxisSystem.hpp"

#include <memory>
#include <string>
#include <vector>

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

	void SetAxisSystem(cd::AxisSystem axisSystem) { m_targetAxisSystem = cd::MoveTemp(axisSystem); }
	cd::AxisSystem& GetAxisSystem() { return m_targetAxisSystem; }
	const cd::AxisSystem& GetAxisSystem() const { return m_targetAxisSystem; }

	void ConvertAxisSystem();

	void Run();

	void AddExtraTextureSearchFolder(const char* pFolderPath) { m_textureSearchFolders.push_back(pFolderPath); }
	bool IsSearchMissingTexturesEnabled() const { return !m_textureSearchFolders.empty(); }

	cd::BitFlags<ProcessorOptions>& GetOptions() { return m_options; }
	const cd::BitFlags<ProcessorOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(ProcessorOptions option) const { return m_options.IsEnabled(option); }

	void CalculateAABBForSceneDatabase();
	void FlattenSceneDatabase();
	void SearchMissingTextures();
	void EmbedTextureFiles();

private:
	IProducer* m_pProducer = nullptr;
	IConsumer* m_pConsumer = nullptr;
	cd::BitFlags<ProcessorOptions> m_options;

	cd::AxisSystem m_targetAxisSystem;
	cd::SceneDatabase* m_pCurrentSceneDatabase;
	std::unique_ptr<cd::SceneDatabase> m_pLocalSceneDatabase;
	std::vector<std::string> m_textureSearchFolders;
};

}