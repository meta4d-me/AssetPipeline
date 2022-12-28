#pragma once

#include "Base/Template.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <string>

struct aiMaterial;
struct aiMesh;

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

	void Execute(cd::SceneDatabase* pSceneDatabase);
	uint32_t GetImportFlags() const;
	void AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	void AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);

	void ActivateDuplicateVertexService() { m_bWantDuplicatedVertex = true; }
	bool IsDuplicateVertexServiceActive() const { return m_bWantDuplicatedVertex; }

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
	// Service flags
	bool m_bWantDuplicatedVertex = false;
	bool m_bWantBoundingBox = false;
	bool m_bWantFlattenHierarchy = false;
	bool m_bWantTriangulate = false;
	bool m_bWantTangentsSpace = false;
	bool m_bWantCleanUnused = false;

	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	std::string m_filePath;
};

}