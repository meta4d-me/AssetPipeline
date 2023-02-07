#include "NodeImpl.h"

namespace cd
{

NodeImpl::NodeImpl(NodeID nodeID, std::string name)
{
	Init(nodeID, cd::MoveTemp(name));
}

void NodeImpl::Init(NodeID nodeID, std::string name)
{
	m_id = nodeID;
	m_name = cd::MoveTemp(name);
}

}