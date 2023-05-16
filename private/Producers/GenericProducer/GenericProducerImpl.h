#pragma once

#include "Base/Template.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <string>

struct aiAnimation;
struct aiLight;
struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;

namespace cd
{

class Mesh;
class SceneDatabase;

}

namespace cdtools
{

class GenericProducerImpl final
{
public:
	GenericProducerImpl() = delete;
	explicit GenericProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	GenericProducerImpl(const GenericProducerImpl&) = delete;
	GenericProducerImpl& operator=(const GenericProducerImpl&) = delete;
	GenericProducerImpl(GenericProducerImpl&&) = delete;
	GenericProducerImpl& operator=(GenericProducerImpl&&) = delete;
	~GenericProducerImpl() = default;

	void SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID);

	void Execute(cd::SceneDatabase* pSceneDatabase);

	void ActivateBoundingBoxService() { m_bWantBoundingBox = true; }
	bool IsBoundingBoxServiceActive() const { return m_bWantBoundingBox; }

	void ActivateFlattenHierarchyService() { m_bWantFlattenHierarchy = true; }
	bool IsFlattenHierarchyServiceActive() const { return m_bWantFlattenHierarchy; }

	void ActivateTriangulateService() { m_bWantTriangulate = true; }
	bool IsTriangulateServiceActive() const { return m_bWantTriangulate; }

	void ActivateTangentsSpaceService() { m_bWantTangentsSpace = true; }
	bool IsTangentsSpaceServiceActive() const { return m_bWantTangentsSpace; }

	void ActivateCleanUnusedService() { m_bWantCleanUnused = true; }
	bool IsCleanUnusedServiceActive() const { return m_bWantCleanUnused; }

	void ActivateSimpleAnimationService() { m_bWantSimpleAnimation = true; }
	bool IsSimpleAnimationServiceActive() const { return m_bWantSimpleAnimation; }

	void ActivateImproveACMRService() { m_bWantSimpleAnimation = true; }
	bool IsImproveACMRServiceActive() const { return m_bWantSimpleAnimation; }

   //  ssssssssssss
	void ActiveSplittextureService() { m_bWantSplitTexture = true; }
	bool IsSimpleActiveSplittextureService() const { return m_bWantSplitTexture; }

private:
	uint32_t GetImportFlags() const;

	// Import different type of objects in the SceneGraph.
	void AddScene(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);
	void AddNodeRecursively(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID);
	void AddLight(cd::SceneDatabase* pSceneDatabase, const aiLight* pSourceLight);
	void AddMaterials(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);
	cd::MaterialID AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	cd::MeshID AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);
	void AddMeshBones(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh, cd::Mesh& mesh);
	void AddAnimation(cd::SceneDatabase* pSceneDatabase, const aiAnimation* pSourceAnimation);

	// Post process stages.
	void RemoveBoneReferenceNodes(cd::SceneDatabase* pSceneDatabase);
	void KeepNodeIDAndIndexSame(cd::SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
	std::string m_folderPath;

	// Service flags
	bool m_bWantBoundingBox = false;
	bool m_bWantFlattenHierarchy = false;
	bool m_bWantTriangulate = false;
	bool m_bWantTangentsSpace = false;
	bool m_bWantCleanUnused = false;
	bool m_bWantSimpleAnimation = false;
	bool m_bWantImproveACMR = false;

	bool m_bWantSplitTexture = false;

	// Generate IDs for different objects
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::AnimationID> m_animationIDGenerator;
	cd::ObjectIDGenerator<cd::TrackID> m_trackIDGenerator;

	std::map<uint32_t, uint32_t> m_nodeIDToNodeIndexLookup;
	std::map<const aiNode*, uint32_t> m_aiNodeToNodeIDLookup;
};

}