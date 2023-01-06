#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Matrix.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class TransformImpl;

class CORE_API Transform final
{
public:
	Transform() = delete;
	explicit Transform(InputArchive& inputArchive);
	explicit Transform(InputArchiveSwapBytes& inputArchive);
	explicit Transform(TransformID transformID, Matrix4x4 transformation);
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;
	Transform(Transform&&);
	Transform& operator=(Transform&&);
	~Transform();
	
	void Init(TransformID transformID, Matrix4x4 transformation);

	const TransformID& GetID() const;

	void SetParentID(uint32_t parentID);
	const TransformID& GetParentID() const;

	void AddChildID(uint32_t childID);
	uint32_t GetChildCount() const;
	std::vector<TransformID>& GetChildIDs();
	const std::vector<TransformID>& GetChildIDs() const;

	void AddMeshID(uint32_t meshID);
	uint32_t GetMeshCount() const;
	std::vector<MeshID>& GetMeshIDs();
	const std::vector<MeshID>& GetMeshIDs() const;

	void SetTransformation(Matrix4x4 transformation);
	Matrix4x4& GetTransformation();
	const Matrix4x4& GetTransformation() const;

	Transform& operator<<(InputArchive& inputArchive);
	Transform& operator<<(InputArchiveSwapBytes& inputArchive);
	const Transform& operator>>(OutputArchive& outputArchive) const;
	const Transform& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TransformImpl* m_pTransformImpl = nullptr;
};

}