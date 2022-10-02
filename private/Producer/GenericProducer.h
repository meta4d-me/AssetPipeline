#pragma once

#include <string>

#include "Producer/IProducer.h"

namespace cdtools
{

// GenericProducer can be used to import all kinds of model formats, such as fbx, glTF, obj. Powered by assimp.
class GenericProducer final : public IProducer
{
public:
	GenericProducer() = delete;
	GenericProducer(std::string filePath);

	virtual void Execute(SceneDatabase* pSceneDatabase) override;

	/// <summary>
	/// Duplicate vertices from the unique vertex in the same positions.
	/// </summary>
	void ActivateDuplicateVertexService(bool flag) { m_bWantDuplicatedVertex = flag; }
	bool IsDuplicateVertexServiceActive() const { return m_bWantDuplicatedVertex; }

	/// <summary>
	/// Generate bounding boxes for every mesh.
	/// </summary>
	void ActivateBoundingBoxService(bool flag) { m_bWantBoundingBox = flag; }
	bool IsBoundingBoxServiceActive() const { return m_bWantBoundingBox; }

	/// <summary>
	/// Flatten the whole model's hierarchy in the same level.
	/// </summary>
	void ActivateFlattenHierarchyService(bool flag) { m_bWantFlattenHierarchy = flag; }
	bool IsFlattenHierarchyServiceActive() const { return m_bWantFlattenHierarchy; }

	/// <summary>
	/// Triangulate the whole model.
	/// </summary>
	void ActivateTriangulateService(bool flag) { m_bWantTriangulate = flag; }
	bool IsTriangulateServiceActive() const { return m_bWantTriangulate; }

private:
	std::string m_filePath;

	// Service flags
	bool m_bWantDuplicatedVertex = false;
	bool m_bWantBoundingBox = false;
	bool m_bWantFlattenHierarchy = false;
	bool m_bWantTriangulate = false;
};

}