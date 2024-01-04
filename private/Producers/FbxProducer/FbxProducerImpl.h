#pragma once

#include "Base/Template.h"
#include "Base/BitFlags.h"
#include "Math/Transform.hpp"
#include "Producers/FbxProducer/FbxProducerOptions.h"
#include "Scene/MaterialTextureType.h"
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
	void TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase);

	std::pair<cd::MaterialID, bool> AllocateMaterialID(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial);
	void ParseMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial);
	void ParseMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase);
	cd::MaterialID ParseMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase);

	cd::NodeID ParseNode(const fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase);

	cd::LightID ParseLight(const fbxsdk::FbxLight* pFbxLight, const char* pLightName, cd::Transform transform, cd::SceneDatabase* pSceneDatabase);

	void ParseMesh(cd::Mesh& mesh, fbxsdk::FbxNode* pSDKNode, fbxsdk::FbxMesh* pFbxMesh);
	
	void ParseSkin();
	std::vector<cd::MorphID> ParseMorphs(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, const std::map<uint32_t, uint32_t>& mapVertexIDToControlPointIndex, cd::SceneDatabase* pSceneDatabase);

	cd::BoneID ParseBone(const fbxsdk::FbxNode* pSDKNode, cd::BoneID parentBoneID, cd::SceneDatabase* pSceneDatabase);
	void ParseAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
	cd::BitFlags<FbxProducerOptions> m_options;

	fbxsdk::FbxManager* m_pSDKManager = nullptr;
	std::unique_ptr<fbxsdk::FbxGeometryConverter> m_pSDKGeometryConverter;

	cd::ObjectIDGenerator<cd::AnimationID> m_animationIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MorphID> m_morphIDGenerator;
	cd::ObjectIDGenerator<cd::SkeletonID> m_skeletonIDGenerator;
	cd::ObjectIDGenerator<cd::SkinID> m_skinIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::TrackID> m_trackIDGenerator;
};

}