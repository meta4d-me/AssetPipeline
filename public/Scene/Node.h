#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Transform.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class NodeImpl;

class CORE_API Node final
{
public:
	DECLARE_SCENE_CLASS(Node);
	explicit Node(NodeID nodeID, std::string name);
	void Init(NodeID nodeID, std::string name);

	EXPORT_SIMPLE_TYPE_APIS(Node, ID);
	EXPORT_SIMPLE_TYPE_APIS(Node, ParentID);
	EXPORT_STRING_TYPE_APIS(Node, Name);
	EXPORT_COMPLEX_TYPE_APIS(Node, Transform);
	EXPORT_VECTOR_TYPE_APIS(Node, ChildID);
	EXPORT_VECTOR_TYPE_APIS(Node, MeshID);
};

}