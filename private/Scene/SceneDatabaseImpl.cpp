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
const Node* SceneDatabaseImpl::GetNodeByName(const std::string& name) const
{
	for (const auto& node : m_nodes)
	{
		if (name == node.GetName())
		{
			return &node;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// Bone
///////////////////////////////////////////////////////////////////
const Bone* SceneDatabaseImpl::GetBoneByName(const std::string& name) const
{
	for (const auto& bone : m_bones)
	{
		if (name == bone.GetName())
		{
			return &bone;
		}
	}

	return nullptr;
}

}
