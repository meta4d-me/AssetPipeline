#include "Scene/Node.h"
#include "NodeImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Node);

Node::Node(NodeID nodeID, std::string name)
{
    m_pNodeImpl = new NodeImpl(nodeID, cd::MoveTemp(name));
}

void Node::Init(NodeID nodeID, std::string name)
{
    m_pNodeImpl->Init(nodeID, cd::MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Node, ID);
PIMPL_SIMPLE_TYPE_APIS(Node, ParentID);
PIMPL_STRING_TYPE_APIS(Node, Name);
PIMPL_COMPLEX_TYPE_APIS(Node, Transform);
PIMPL_VECTOR_TYPE_APIS(Node, ChildID);
PIMPL_VECTOR_TYPE_APIS(Node, MeshID);

}