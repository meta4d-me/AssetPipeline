#pragma once

#include "Base/Template.h"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <string>

namespace fbxsdk
{

class FbxManager;
class FbxSurfaceMaterial;
class FbxNode;
class FbxScene;

}

namespace cd
{

class Bone;
class Material;
class Node;
class SceneDatabase;

}

namespace cdtools
{

class FbxProducerImpl final
{
public:
	FbxProducerImpl() = delete;
	explicit FbxProducerImpl(std::string filePath);
	FbxProducerImpl(const FbxProducerImpl&) = delete;
	FbxProducerImpl& operator=(const FbxProducerImpl&) = delete;
	FbxProducerImpl(FbxProducerImpl&&) = delete;
	FbxProducerImpl& operator=(FbxProducerImpl&&) = delete;
	~FbxProducerImpl();

	void Execute(cd::SceneDatabase* pSceneDatabase);

	void SetWantImportMaterial(bool flag) { m_importMaterial = flag; }
	bool WantImportMaterial() const { return m_importMaterial; }

	void SetWantImportTexture(bool flag) { m_importTexture = flag; }
	bool WantImportTexture() const { return m_importTexture; }

	void SetWantImportSkinMesh(bool flag) { m_importSkinMesh = flag; }
	bool WantImportSkinMesh() const { return m_importSkinMesh; }

	void SetWantImportAnimation(bool flag) { m_importAnimation = flag; }
	bool WantImportAnimation() const { return m_importAnimation; }

private:
	int GetSceneNodeCount(const fbxsdk::FbxNode* pSceneNode);
	void TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);

	int GetSceneBoneCount(const fbxsdk::FbxNode* pSceneNode);
	void TraverseBoneRecursively(fbxsdk::FbxNode* pSDKNode, cd::Bone* pParentNode, cd::SceneDatabase* pSceneDatabase);

	void AddMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial);
	void AddMaterialTexture(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pTextureName, cd::MaterialTextureType textureType, cd::Material* pMaterial, cd::SceneDatabase* pSceneDatabase);
	cd::MaterialID AddMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase);

	cd::NodeID AddNode(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);
	cd::MeshID AddMesh(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);

	cd::BoneID AddBone(const fbxsdk::FbxNode* pSDKNode, cd::Bone* pParentNode, cd::SceneDatabase* pSceneDatabase);
	//cd::TrackID AddTrack(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);
	//cd::AnimationID AddAnimation(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);

private:
	bool m_importMaterial = true;
	bool m_importTexture = true;
	bool m_importSkinMesh = true;
	bool m_importAnimation = true;

	std::string m_filePath;
	fbxsdk::FbxManager* m_pSDKManager = nullptr;

	std::map<int32_t, uint32_t> m_fbxMaterialIndexToMaterialID;
	std::vector<std::string> m_textureSearchFolders;

	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
};

}