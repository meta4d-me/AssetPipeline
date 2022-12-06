#pragma once

#include "Producer/BaseProducer.h"

#include "Scene/ObjectIDGenerator.h"

struct aiMaterial;
struct aiMesh;

namespace cdtools
{

// GenericProducer can be used to import all kinds of model formats, such as fbx, glTF, obj. Powered by assimp.
class GenericProducer final : public BaseProducer
{
public:
	using BaseProducer::BaseProducer;
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	/// <summary>
	/// Duplicate vertices from the unique vertex in the same positions.
	/// </summary>
	void ActivateDuplicateVertexService() { m_bWantDuplicatedVertex = true; }
	bool IsDuplicateVertexServiceActive() const { return m_bWantDuplicatedVertex; }

	/// <summary>
	/// Generate bounding boxes for every mesh.
	/// </summary>
	void ActivateBoundingBoxService() { m_bWantBoundingBox = true; }
	bool IsBoundingBoxServiceActive() const { return m_bWantBoundingBox; }

	/// <summary>
	/// Flatten the whole model's hierarchy in the same level.
	/// </summary>
	void ActivateFlattenHierarchyService() { m_bWantFlattenHierarchy = true; }
	bool IsFlattenHierarchyServiceActive() const { return m_bWantFlattenHierarchy; }

	/// <summary>
	/// Triangulate the whole model.
	/// </summary>
	void ActivateTriangulateService() { m_bWantTriangulate = true; }
	bool IsTriangulateServiceActive() const { return m_bWantTriangulate; }

	/// <summary>
	/// Generate tangent space vertex coordinates.
	/// </summary>
	void ActivateTangentsSpaceService() { m_bWantTangentsSpace = true; }
	bool IsTangentsSpaceServiceActive() const { return m_bWantTangentsSpace; }

	/// <summary>
	/// Clean unused data.
	/// </summary>
	void ActivateCleanUnusedService() { m_bWantCleanUnused = true; }
	bool IsCleanUnusedServiceActive() const { return m_bWantCleanUnused; }

private:
	uint32_t GetImportFlags() const;
	void AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	void AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);

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
};

}