#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Matrix.hpp"
#include "Math/Transform.hpp"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class NodeImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Node);

	explicit NodeImpl(NodeID nodeID, std::string name);
	void Init(NodeID nodeID, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(Node, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Node, ParentID);
	IMPLEMENT_COMPLEX_TYPE_APIS(Node, Transform);
	IMPLEMENT_VECTOR_TYPE_APIS(Node, ChildID);
	IMPLEMENT_VECTOR_TYPE_APIS(Node, MeshID);
	IMPLEMENT_STRING_TYPE_APIS(Node, Name);

	template<bool SwapBytesOrder>
	NodeImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t nodeID;
		uint32_t parentID;
		std::string nodeName;
		Transform transform;
		uint32_t childCount;
		uint32_t meshCount;

		inputArchive >> nodeID >> parentID;
		inputArchive >> nodeName >> transform;
		inputArchive >> childCount >> meshCount;

		Init(NodeID(nodeID), cd::MoveTemp(nodeName));
		SetParentID(NodeID(parentID));
		SetTransform(cd::MoveTemp(transform));

		GetChildIDs().resize(childCount);
		inputArchive.ImportBuffer(GetChildIDs().data());

		GetMeshIDs().resize(meshCount);
		inputArchive.ImportBuffer(GetMeshIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const NodeImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetParentID().Data();
		outputArchive << GetName() << GetTransform();
		outputArchive << GetChildIDCount() << GetMeshIDCount();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive.ExportBuffer(GetMeshIDs().data(), GetMeshIDs().size());

		return *this;
	}
};

}