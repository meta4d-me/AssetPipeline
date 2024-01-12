#pragma once

#include "Base/Template.h"
#include "Base/BitFlags.h"
#include "Consumers/FbxConsumer/FbxConsumerOptions.h"
#include "Scene/SceneDatabase.h"

namespace fbxsdk
{

class FbxManager;
class FbxScene;

}

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class FbxConsumerImpl final
{
public:
	FbxConsumerImpl() = delete;
	explicit FbxConsumerImpl(std::string filePath);
	FbxConsumerImpl(const FbxConsumerImpl&) = delete;
	FbxConsumerImpl& operator=(const FbxConsumerImpl&) = delete;
	FbxConsumerImpl(FbxConsumerImpl&&) = delete;
	FbxConsumerImpl& operator=(FbxConsumerImpl&&) = delete;
	~FbxConsumerImpl();

	void Execute(const cd::SceneDatabase* pSceneDatabase);
	fbxsdk::FbxScene* CreateScene(const cd::SceneDatabase* pSceneDatabase);
	void ExportMesh(fbxsdk::FbxScene* pScene, const cd::Mesh& mesh, const cd::SceneDatabase* pSceneDatabase);
	bool ExportFbxFile(fbxsdk::FbxScene* pScene);

	cd::BitFlags<FbxConsumerOptions>& GetOptions() { return m_options; }
	const cd::BitFlags<FbxConsumerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(FbxConsumerOptions option) const { return m_options.IsEnabled(option); }

private:
	cd::BitFlags<FbxConsumerOptions> m_options;
	std::string m_filePath;

	fbxsdk::FbxManager*		m_pSDKManager = nullptr;
};

}