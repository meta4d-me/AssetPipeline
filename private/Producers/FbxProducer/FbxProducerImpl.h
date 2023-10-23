#pragma once

#include "Base/Template.h"
#include "Math/Transform.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <fbxsdk/core/arch/fbxtypes.h>

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
class FbxSurfaceMaterial;
class FbxAnimStack;
class FbxScene;
class FbxTime;

}

namespace cd
{

class Bone;
class Material;
class Mesh;
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

	void SetWantImportLight(bool flag) { m_importLight = flag; }
	bool WantImportLight() const { return m_importLight; }

	void SetWantTriangulate(bool flag) { m_bWantTriangulate = flag; }
	bool IsTriangulateActive() const { return m_bWantTriangulate; }

private:
	int GetSceneNodeCount(const fbxsdk::FbxNode* pSceneNode);
	void TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase);

	void AddMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial);
	void AddMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase);
	cd::MaterialID AddMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase);

	cd::NodeID AddNode(const fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase);
	cd::LightID AddLight(const fbxsdk::FbxLight* pFbxLight, const char* pLightName, cd::Transform transform, cd::SceneDatabase* pSceneDatabase);
	cd::MeshID AddMesh(const fbxsdk::FbxMesh* pFbxMesh, const char* pMeshName, std::optional<int32_t> optMaterialIndex, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase);
	std::vector<cd::MorphID> AddMorphs(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, const std::map<uint32_t, uint32_t>& mapVertexIDToControlPointIndex, cd::SceneDatabase* pSceneDatabase);

	cd::BoneID AddBone(const fbxsdk::FbxNode* pSDKNode, cd::BoneID parentBoneID, cd::SceneDatabase* pSceneDatabase);
	//cd::TrackID AddTrack(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase);
	cd::AnimationID AddAnimation(fbxsdk::FbxNode* pSDKNode, fbxsdk::FbxScene* pSDKScene, cd::SceneDatabase* pSceneDatabase);

	void ProcessAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase);
private:
	bool m_importMaterial = true;
	bool m_importTexture = true;
	bool m_importSkinMesh = true;
	bool m_importAnimation = true;
	bool m_importLight = true;
	bool m_bWantTriangulate = true;

	std::string m_filePath;
	fbxsdk::FbxManager* m_pSDKManager = nullptr;
	std::unique_ptr<fbxsdk::FbxGeometryConverter> m_pSDKGeometryConverter;

	std::map<int32_t, uint32_t> m_fbxMaterialIndexToMaterialID;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MorphID> m_morphIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;
	cd::ObjectIDGenerator<cd::AnimationID> m_animationIDGenerator;
	cd::ObjectIDGenerator<cd::TrackID> m_trackIDGenerator;
};

}