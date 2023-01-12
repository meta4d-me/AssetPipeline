#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Matrix.hpp"
#include "Math/Transform.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class NodeImpl final
{
public:
	NodeImpl() = delete;
	template<bool SwapBytesOrder>
	explicit NodeImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit NodeImpl(NodeID nodeID);
	NodeImpl(const NodeImpl&) = default;
	NodeImpl& operator=(const NodeImpl&) = default;
	NodeImpl(NodeImpl&&) = default;
	NodeImpl& operator=(NodeImpl&&) = default;
	~NodeImpl() = default;

	void Init(NodeID nodeID);

	const NodeID& GetID() const { return m_id; }

	void SetParentID(uint32_t parentID) { m_parentID.Set(parentID); }
	const NodeID& GetParentID() const { return m_parentID; }

	void AddChildID(uint32_t childID) { m_childIDs.push_back(NodeID(childID)); }
	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_childIDs.size()); }
	std::vector<NodeID>& GetChildIDs() { return m_childIDs; }
	const std::vector<NodeID>& GetChildIDs() const { return m_childIDs; }

	void AddMeshID(uint32_t meshID) { m_meshIDs.push_back(MeshID(meshID)); }
	uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_meshIDs.size()); }
	std::vector<MeshID>& GetMeshIDs() { return m_meshIDs; }
	const std::vector<MeshID>& GetMeshIDs() const { return m_meshIDs; }

	void SetTransform(Transform transform) { m_transform = cd::MoveTemp(transform); }
	Transform& GetTransform() { return m_transform; }
	const Transform& GetTransform() const { return m_transform; }

	template<bool SwapBytesOrder>
	NodeImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t nodeID;
		uint32_t parentID;
		Transform transform;
		uint32_t childCount;
		uint32_t meshCount;

		inputArchive >> nodeID >> parentID;
		inputArchive >> transform;
		inputArchive >> childCount >> meshCount;

		Init(NodeID(nodeID));
		SetParentID(parentID);
		SetTransform(cd::MoveTemp(transform));

		m_childIDs.resize(childCount);
		inputArchive.ImportBuffer(GetChildIDs().data());

		m_meshIDs.resize(meshCount);
		inputArchive.ImportBuffer(GetMeshIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const NodeImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetParentID().Data();
		outputArchive << GetTransform();
		outputArchive << GetChildCount() << GetMeshCount();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive.ExportBuffer(GetMeshIDs().data(), GetMeshIDs().size());

		return *this;
	}

private:
	NodeID m_id;
	NodeID m_parentID;
	std::vector<NodeID> m_childIDs;
	std::vector<MeshID> m_meshIDs;
	
	Transform m_transform;
};

}