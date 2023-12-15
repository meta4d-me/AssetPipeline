#include "NodeImpl.h"

namespace cd
{

NodeImpl::NodeImpl(NodeID nodeID, std::string name)
{
	Init(nodeID, cd::MoveTemp(name));
}

void NodeImpl::Init(NodeID nodeID, std::string name)
{
	SetID(nodeID);
	SetName(cd::MoveTemp(name));
}

}