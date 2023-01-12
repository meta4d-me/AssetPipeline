#include "NodeImpl.h"

namespace cd
{

NodeImpl::NodeImpl(NodeID nodeID)
{
	Init(nodeID);
}

void NodeImpl::Init(NodeID nodeID)
{
	m_id = nodeID;
}

}