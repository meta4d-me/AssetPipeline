#include "SceneDatabaseImpl.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
{

SceneDatabaseImpl::SceneDatabaseImpl()
{
	m_aabb.Clear();
}

///////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////
const Node* SceneDatabaseImpl::GetNodeByName(const char* pName) const
{
	for (const auto& node : m_nodes)
	{
		if (0 == strcmp(pName, node.GetName()))
		{
			return &node;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// Bone
///////////////////////////////////////////////////////////////////
const Bone* SceneDatabaseImpl::GetBoneByName(const char* pName) const
{
	for (const auto& bone : m_bones)
	{
		if (0 == strcmp(pName, bone.GetName()))
		{
			return &bone;
		}
	}

	return nullptr;
}

}