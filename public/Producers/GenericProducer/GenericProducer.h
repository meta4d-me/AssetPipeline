#pragma once

#include "Framework/IProducer.h"

#include <cstdint>

struct aiMaterial;
struct aiMesh;
struct aiNode;

namespace cdtools
{

class GenericProducerImpl;

// GenericProducer can be used to import all kinds of model formats, such as fbx, glTF, obj. Powered by assimp.
class TOOL_API GenericProducer final : public IProducer
{
public:
	GenericProducer() = delete;
	explicit GenericProducer(const char* pFilePath);
	GenericProducer(const GenericProducer&) = delete;
	GenericProducer& operator=(const GenericProducer&) = delete;
	GenericProducer(GenericProducer&&) = delete;
	GenericProducer& operator=(GenericProducer&&) = delete;
	virtual ~GenericProducer();

	void SetSceneDatabaseIDs(uint32_t transformID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID);
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	/// <summary>
	/// Duplicate vertices from the unique vertex in the same positions.
	/// </summary>
	void ActivateDuplicateVertexService();
	bool IsDuplicateVertexServiceActive() const;

	/// <summary>
	/// Generate bounding boxes for every mesh.
	/// </summary>
	void ActivateBoundingBoxService();
	bool IsBoundingBoxServiceActive() const;

	/// <summary>
	/// Flatten the whole model's hierarchy in the same level.
	/// </summary>
	void ActivateFlattenHierarchyService();
	bool IsFlattenHierarchyServiceActive() const;

	/// <summary>
	/// Triangulate the whole model.
	/// </summary>
	void ActivateTriangulateService();
	bool IsTriangulateServiceActive() const;

	/// <summary>
	/// Generate tangent space vertex coordinates.
	/// </summary>
	void ActivateTangentsSpaceService();
	bool IsTangentsSpaceServiceActive() const;

	/// <summary>
	/// Clean unused data.
	/// </summary>
	void ActivateCleanUnusedService();
	bool IsCleanUnusedServiceActive() const;

private:
	GenericProducerImpl* m_pGenericProducerImpl;
};

}