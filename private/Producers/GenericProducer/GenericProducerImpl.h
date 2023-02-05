#pragma once

#include "Base/Template.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <string>

struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;

namespace cd
{

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

	uint32_t GetImportFlags() const;
	cd::MaterialID AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	cd::MeshID AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);
	cd::NodeID AddNode(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID);

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

private:
	std::string m_filePath;
	std::string m_folderPath;

	// Service flags
	bool m_bWantBoundingBox = false;
	bool m_bWantFlattenHierarchy = false;
	bool m_bWantTriangulate = false;
	bool m_bWantTangentsSpace = false;
	bool m_bWantCleanUnused = false;

	// Generate IDs for different objects
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;

	std::map<const aiNode*, uint32_t> m_aiNodeToNodeIDLookup;
};

}