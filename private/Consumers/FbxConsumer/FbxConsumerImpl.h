#pragma once

#include "Base/Template.h"
#include "Base/BitFlags.h"
#include "Consumers/FbxConsumer/FbxConsumerOptions.h"
#include "Scene/SceneDatabase.h"

namespace fbxsdk
{

class FbxFileTexture;
class FbxManager;
class FbxNode;
class FbxScene;
class FbxSurfaceMaterial;

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

	cd::BitFlags<FbxConsumerOptions>& GetOptions() { return m_options; }
	const cd::BitFlags<FbxConsumerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(FbxConsumerOptions option) const { return m_options.IsEnabled(option); }

private:
	fbxsdk::FbxScene* CreateScene(const cd::SceneDatabase* pSceneDatabase);
	fbxsdk::FbxFileTexture* ExportTexture(fbxsdk::FbxScene* pScene, cd::TextureID textureID, const cd::SceneDatabase* pSceneDatabase);
	fbxsdk::FbxSurfaceMaterial* ExportMaterial(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode, cd::MaterialID materialID, const cd::SceneDatabase* pSceneDatabase);
	void ExportMesh(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode, const cd::Mesh& mesh, const cd::SceneDatabase* pSceneDatabase);
	fbxsdk::FbxNode* ExportNodeRecursively(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pParentNode, cd::NodeID nodeID, const cd::SceneDatabase* pSceneDatabase);
	fbxsdk::FbxNode* ExportNode(fbxsdk::FbxScene* pScene, const char* pName, const cd::Transform& transform, const cd::SceneDatabase* pSceneDatabase);
	bool ExportFbxFile(fbxsdk::FbxScene* pScene);

private:
	cd::BitFlags<FbxConsumerOptions> m_options;
	std::string m_filePath;

	fbxsdk::FbxManager*		m_pSDKManager = nullptr;
};

}