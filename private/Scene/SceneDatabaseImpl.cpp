#include "SceneDatabaseImpl.h"

#include "Base/Template.h"

#include <cassert>

namespace cd
{

SceneDatabaseImpl::SceneDatabaseImpl()
{
	m_aabb.Clear();

	// By default, it will be CDEngine for conveniences.
	// If you are outside user in other platforms, you can set it by yourself.
	// TODO : add a update method to change axis system for SceneDatabase dynamicly.
	m_axisSystem = AxisSystem::CDEngine();
	m_unit = Unit::CenterMeter;
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

///////////////////////////////////////////////////////////////////
// Track
///////////////////////////////////////////////////////////////////
const Track* SceneDatabaseImpl::GetTrackByName(const char* pName) const
{
	for (const auto& track : m_tracks)
	{
		if (0 == strcmp(pName, track.GetName()))
		{
			return &track;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::UpdateAABB()
{
	cd::AABB sceneAABB(0.0f, 0.0f);
	for (const auto& mesh : GetMeshes())
	{
		sceneAABB.Merge(mesh.GetAABB());
	}
	SetAABB(cd::MoveTemp(sceneAABB));
}

}
