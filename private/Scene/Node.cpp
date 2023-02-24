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

void Node::SetID(NodeID nodeID)
{
    m_pNodeImpl->SetID(nodeID);
}

const NodeID& Node::GetID() const
{
    return m_pNodeImpl->GetID();
}

void Node::SetName(std::string name)
{
    return m_pNodeImpl->SetName(cd::MoveTemp(name));
}

const char* Node::GetName() const
{
    return m_pNodeImpl->GetName().c_str();
}

void Node::SetParentID(uint32_t parentID)
{
    m_pNodeImpl->SetParentID(parentID);
}

const NodeID& Node::GetParentID() const
{
    return m_pNodeImpl->GetParentID();
}

void Node::AddChildID(uint32_t childID)
{
    m_pNodeImpl->AddChildID(childID);
}

uint32_t Node::GetChildCount() const
{
    return m_pNodeImpl->GetChildCount();
}

std::vector<NodeID>& Node::GetChildIDs()
{
    return m_pNodeImpl->GetChildIDs();
}

const std::vector<NodeID>& Node::GetChildIDs() const
{
    return m_pNodeImpl->GetChildIDs();
}

void Node::AddMeshID(uint32_t meshID)
{
    m_pNodeImpl->AddMeshID(meshID);
}

uint32_t Node::GetMeshCount() const
{
    return m_pNodeImpl->GetMeshCount();
}

std::vector<MeshID>& Node::GetMeshIDs()
{
    return m_pNodeImpl->GetMeshIDs();
}

const std::vector<MeshID>& Node::GetMeshIDs() const
{
    return m_pNodeImpl->GetMeshIDs();
}

void Node::SetTransform(Transform transform)
{
    return m_pNodeImpl->SetTransform(cd::MoveTemp(transform));
}

Transform& Node::GetTransform()
{
    return m_pNodeImpl->GetTransform();
}

const Transform& Node::GetTransform() const
{
    return m_pNodeImpl->GetTransform();
}

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