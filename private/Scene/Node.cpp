#include "Scene/Node.h"
#include "NodeImpl.h"

namespace cd
{

Node::Node(InputArchive& inputArchive)
{
    m_pNodeImpl = new NodeImpl(inputArchive);
}

Node::Node(InputArchiveSwapBytes& inputArchive)
{
    m_pNodeImpl = new NodeImpl(inputArchive);
}

Node::Node(NodeID nodeID, std::string name)
{
    m_pNodeImpl = new NodeImpl(nodeID, cd::MoveTemp(name));
}

Node::Node(Node&& rhs)
{
    *this = cd::MoveTemp(rhs);
}

Node& Node::operator=(Node&& rhs)
{
    std::swap(m_pNodeImpl, rhs.m_pNodeImpl);
    return *this;
}

Node::~Node()
{
    if (m_pNodeImpl)
    {
        delete m_pNodeImpl;
        m_pNodeImpl = nullptr;
    }
}

void Node::Init(NodeID nodeID, std::string name)
{
    m_pNodeImpl->Init(nodeID, cd::MoveTemp(name));
}

PIMPL_SIMPLE_TYPE_APIS(Node, ID);
PIMPL_SIMPLE_TYPE_APIS(Node, ParentID);
PIMPL_COMPLEX_TYPE_APIS(Node, Transform);
PIMPL_VECTOR_TYPE_APIS(Node, ChildID);
PIMPL_VECTOR_TYPE_APIS(Node, MeshID);
PIMPL_STRING_TYPE_APIS(Node, Name);

Node& Node::operator<<(InputArchive& inputArchive)
{
    *m_pNodeImpl << inputArchive;
    return *this;
}

Node& Node::operator<<(InputArchiveSwapBytes& inputArchive)
{
    *m_pNodeImpl << inputArchive;
    return *this;
}

const Node& Node::operator>>(OutputArchive& outputArchive) const
{
    *m_pNodeImpl >> outputArchive;
    return *this;
}

const Node& Node::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
    *m_pNodeImpl >> outputArchive;
    return *this;
}

}