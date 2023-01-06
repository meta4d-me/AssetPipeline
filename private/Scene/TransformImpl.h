#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Matrix.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class TransformImpl final
{
public:
	TransformImpl() = delete;
	template<bool SwapBytesOrder>
	explicit TransformImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit TransformImpl(TransformID transformID);
	TransformImpl(const TransformImpl&) = default;
	TransformImpl& operator=(const TransformImpl&) = default;
	TransformImpl(TransformImpl&&) = default;
	TransformImpl& operator=(TransformImpl&&) = default;
	~TransformImpl() = default;

	void Init(TransformID transformID);

	const TransformID& GetID() const { return m_id; }

	void SetParentID(uint32_t parentID) { m_parentID.Set(parentID); }
	const TransformID& GetParentID() const { return m_parentID; }

	void AddChildID(uint32_t childID) { m_childIDs.push_back(TransformID(childID)); }
	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_childIDs.size()); }
	std::vector<TransformID>& GetChildIDs() { return m_childIDs; }
	const std::vector<TransformID>& GetChildIDs() const { return m_childIDs; }

	void AddMeshID(uint32_t meshID) { m_meshIDs.push_back(MeshID(meshID)); }
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshIDs.size()); }
	std::vector<MeshID>& GetMeshIDs() { return m_meshIDs; }
	const std::vector<MeshID>& GetMeshIDs() const { return m_meshIDs; }

	void SetTransformation(Matrix4x4 transformation) { m_transformation = cd::MoveTemp(transformation); }
	Matrix4x4& GetTransformation() { return m_transformation; }
	const Matrix4x4& GetTransformation() const { return m_transformation; }

	template<bool SwapBytesOrder>
	TransformImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t transformID;
		uint32_t parentID;
		uint32_t childCount;
		uint32_t meshCount;
		Matrix4x4 transformation;

		inputArchive >> transformID;
		inputArchive >> parentID;
		inputArchive >> childCount;
		inputArchive >> meshCount;

		Init(TransformID(transformID));
		m_childIDs.resize(childCount);
		m_meshIDs.resize(meshCount);

		inputArchive.ImportBuffer(GetChildIDs().data());
		inputArchive.ImportBuffer(GetMeshIDs().data());
		inputArchive.ImportBuffer(transformation.Begin());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TransformImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data();
		outputArchive << GetParentID().Data();
		outputArchive << GetChildCount();
		outputArchive << GetMeshCount();

		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive.ExportBuffer(GetMeshIDs().data(), GetMeshIDs().size());
		outputArchive.ExportBuffer(GetTransformation().Begin(), GetTransformation().Size);

		return *this;
	}

private:
	TransformID m_id;
	TransformID m_parentID;
	std::vector<TransformID> m_childIDs;
	std::vector<MeshID> m_meshIDs;
	Matrix4x4 m_transformation;
};

}