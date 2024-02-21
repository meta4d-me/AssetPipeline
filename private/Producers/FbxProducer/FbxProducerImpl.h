#pragma once

#include "Base/Template.h"
#include "Base/BitFlags.h"
#include "Producers/FbxProducer/FbxProducerOptions.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace fbxsdk
{

class FbxBlendShape;
class FbxGeometryConverter;
class FbxLight;
class FbxManager;
class FbxMesh;
class FbxNode;
class FbxProperty;
class FbxScene;
class FbxSkin;
class FbxSurfaceMaterial;

}

namespace cd
{

class Material;
class Mesh;
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

	cd::BitFlags<FbxProducerOptions>& GetOptions() { return m_options; }
	const cd::BitFlags<FbxProducerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(FbxProducerOptions option) const { return m_options.IsEnabled(option); }

private:
	struct FbxSceneInfo
	{
		fbxsdk::FbxNode* rootNode;
		std::vector<fbxsdk::FbxNode*> transformNodes;
		std::vector<fbxsdk::FbxNode*> skeletonRootBones;
		std::vector<std::vector<fbxsdk::FbxNode*>> skeletalMeshArrays;
		std::vector<fbxsdk::FbxNode*> staticMeshes;
		std::vector<fbxsdk::FbxSurfaceMaterial*> surfaceMaterials;
		std::vector<fbxsdk::FbxLight*> lights;
	};

	// Scene
	void BuildSceneInfo(fbxsdk::FbxScene* pScene, FbxSceneInfo& sceneInfo);
	void TraverseSceneRecursively(fbxsdk::FbxNode* pNode, FbxSceneInfo& sceneInfo);

	// Node
	cd::NodeID AllocateNodeID(const fbxsdk::FbxNode* pSDKNode);
	cd::NodeID ImportNode(const fbxsdk::FbxNode* pSDKNode, cd::SceneDatabase* pSceneDatabase);
	
	// Light
	cd::LightID ImportLight(const fbxsdk::FbxLight* pFbxLight, cd::SceneDatabase* pSceneDatabase);

	// Material
	std::pair<cd::MaterialID, bool> AllocateMaterialID(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial);
	void ImportMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial);
	void ImportMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase);
	cd::MaterialID ImportMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase);

	// Mesh
	cd::MeshID ImportMesh(const fbxsdk::FbxMesh* pFbxMesh, cd::SceneDatabase* pSceneDatabase);

	// SkeletalMesh
	void ImportSkeletonBones(fbxsdk::FbxScene* pScene, const std::vector<fbxsdk::FbxNode*>& skeletonMeshNodes, cd::SceneDatabase* pSceneDatabase);
	cd::MeshID ImportSkeletalMesh(const fbxsdk::FbxMesh* pFbxMesh, cd::SceneDatabase* pSceneDatabase);
	cd::SkinID ImportSkin(const fbxsdk::FbxSkin* pSkin, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase);

	// BlendShape
	cd::BlendShapeID ImportBlendShape(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase);
	void AssociateMeshWithBlendShape(fbxsdk::FbxMesh* pMesh, cd::MeshID meshID, cd::SceneDatabase* pSceneDatabase);

	// Animation
	void ImportAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase);

private:
	cd::BitFlags<FbxProducerOptions> m_options;
	std::string m_filePath;
	
	fbxsdk::FbxManager* m_pSDKManager = nullptr;
	std::unique_ptr<fbxsdk::FbxGeometryConverter> m_pSDKGeometryConverter;

	cd::ObjectIDGenerator<cd::AnimationID> m_animationIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::BlendShapeID> m_blendShapeIDGenerator;
	cd::ObjectIDGenerator<cd::MorphID> m_morphIDGenerator;
	cd::ObjectIDGenerator<cd::SkeletonID> m_skeletonIDGenerator;
	cd::ObjectIDGenerator<cd::SkinID> m_skinIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::TrackID> m_trackIDGenerator;
};

}