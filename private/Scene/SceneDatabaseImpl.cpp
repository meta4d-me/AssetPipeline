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
void SceneDatabaseImpl::SetNodeCount(uint32_t nodeCount)
{
	m_nodes.reserve(nodeCount);
}

void SceneDatabaseImpl::AddNode(Node node)
{
	m_nodes.emplace_back(MoveTemp(node));
}

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
void SceneDatabaseImpl::SetBoneCount(uint32_t boneCount)
{
	m_bones.reserve(boneCount);
}

void SceneDatabaseImpl::AddBone(Bone bone)
{
	m_bones.emplace_back(MoveTemp(bone));
}

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

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMeshCount(uint32_t meshCount)
{
	m_meshes.reserve(meshCount);
}

void SceneDatabaseImpl::AddMesh(Mesh mesh)
{
	m_meshes.emplace_back(MoveTemp(mesh));
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetMaterialCount(uint32_t materialCount)
{
	m_materials.reserve(materialCount);
}

void SceneDatabaseImpl::AddMaterial(Material material)
{
	m_materials.emplace_back(MoveTemp(material));
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetTextureCount(uint32_t textureCount)
{
	m_textures.reserve(textureCount);
}

void SceneDatabaseImpl::AddTexture(Texture texture)
{
	m_textures.emplace_back(MoveTemp(texture));
}

///////////////////////////////////////////////////////////////////
// Light
///////////////////////////////////////////////////////////////////
void SceneDatabaseImpl::SetLightCount(uint32_t lightCount)
{
	m_lights.reserve(lightCount);
}

void SceneDatabaseImpl::AddLight(Light light)
{
	m_lights.emplace_back(MoveTemp(light));
}

}