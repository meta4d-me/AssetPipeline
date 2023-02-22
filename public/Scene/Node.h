#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Transform.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class NodeImpl;

class CORE_API Node final
{
public:
	Node() = delete;
	explicit Node(InputArchive& inputArchive);
	explicit Node(InputArchiveSwapBytes& inputArchive);
	explicit Node(NodeID nodeID, std::string name);
	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;
	Node(Node&&);
	Node& operator=(Node&&);
	~Node();
	
	void Init(NodeID nodeID, std::string name);

	void SetID(NodeID nodeID);
	const NodeID& GetID() const;

	void SetName(std::string name);
	std::string& GetName();
	const std::string& GetName() const;

	void SetParentID(uint32_t parentID);
	const NodeID& GetParentID() const;

	void AddChildID(uint32_t childID);
	uint32_t GetChildCount() const;
	std::vector<NodeID>& GetChildIDs();
	const std::vector<NodeID>& GetChildIDs() const;

	void AddMeshID(uint32_t meshID);
	uint32_t GetMeshCount() const;
	std::vector<MeshID>& GetMeshIDs();
	const std::vector<MeshID>& GetMeshIDs() const;

	void SetTransform(Transform transform);
	Transform& GetTransform();
	const Transform& GetTransform() const;

	Node& operator<<(InputArchive& inputArchive);
	Node& operator<<(InputArchiveSwapBytes& inputArchive);
	const Node& operator>>(OutputArchive& outputArchive) const;
	const Node& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	NodeImpl* m_pNodeImpl = nullptr;
};

}