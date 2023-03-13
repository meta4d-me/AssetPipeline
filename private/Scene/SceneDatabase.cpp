#include "Scene/SceneDatabase.h"
#include "SceneDatabaseImpl.h"

#include <cassert>

namespace cd
{

SceneDatabase::SceneDatabase()
{
	m_pSceneDatabaseImpl = new SceneDatabaseImpl();
}

SceneDatabase::SceneDatabase(SceneDatabase&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

SceneDatabase& SceneDatabase::operator=(SceneDatabase&& rhs)
{
	std::swap(m_pSceneDatabaseImpl, rhs.m_pSceneDatabaseImpl);
	return *this;
}

SceneDatabase::~SceneDatabase()
{
	if (m_pSceneDatabaseImpl)
	{
		delete m_pSceneDatabaseImpl;
		m_pSceneDatabaseImpl = nullptr;
	}
}

void SceneDatabase::SetName(const char* pName)
{
	m_pSceneDatabaseImpl->SetName(pName);
}

const char* SceneDatabase::GetName() const
{
	return m_pSceneDatabaseImpl->GetName().c_str();
}

void SceneDatabase::SetAABB(AABB aabb)
{
	return m_pSceneDatabaseImpl->SetAABB(cd::MoveTemp(aabb));
}

AABB& SceneDatabase::GetAABB()
{
	return m_pSceneDatabaseImpl->GetAABB();
}

const AABB& SceneDatabase::GetAABB() const
{
	return m_pSceneDatabaseImpl->GetAABB();
}

///////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddNode(Node node)
{
	return m_pSceneDatabaseImpl->AddNode(cd::MoveTemp(node));
}

std::vector<Node>& SceneDatabase::GetNodes()
{
	return m_pSceneDatabaseImpl->GetNodes();
}

const std::vector<Node>& SceneDatabase::GetNodes() const
{
	return m_pSceneDatabaseImpl->GetNodes();
}

void SceneDatabase::SetNodeCount(uint32_t nodeCount)
{
	return m_pSceneDatabaseImpl->SetNodeCount(nodeCount);
}

const Node& SceneDatabase::GetNode(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetNode(index);
}

const Node* SceneDatabase::GetNodeByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetNodeByName(pName);
}

uint32_t SceneDatabase::GetNodeCount() const
{
	return m_pSceneDatabaseImpl->GetNodeCount();
}

///////////////////////////////////////////////////////////////////
// Mesh
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddMesh(Mesh mesh)
{
	return m_pSceneDatabaseImpl->AddMesh(cd::MoveTemp(mesh));
}

std::vector<Mesh>& SceneDatabase::GetMeshes()
{
	return m_pSceneDatabaseImpl->GetMeshes();
}

const std::vector<Mesh>& SceneDatabase::GetMeshes() const
{
	return m_pSceneDatabaseImpl->GetMeshes();
}

void SceneDatabase::SetMeshCount(uint32_t meshCount)
{
	return m_pSceneDatabaseImpl->SetMeshCount(meshCount);
}

const Mesh& SceneDatabase::GetMesh(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetMesh(index);
}

uint32_t SceneDatabase::GetMeshCount() const
{
	return m_pSceneDatabaseImpl->GetMeshCount();
}

///////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddMaterial(Material material)
{
	return m_pSceneDatabaseImpl->AddMaterial(cd::MoveTemp(material));
}

std::vector<Material>& SceneDatabase::GetMaterials()
{
	return m_pSceneDatabaseImpl->GetMaterials();
}

const std::vector<Material>& SceneDatabase::GetMaterials() const
{
	return m_pSceneDatabaseImpl->GetMaterials();
}

void SceneDatabase::SetMaterialCount(uint32_t materialCount)
{
	return m_pSceneDatabaseImpl->SetMaterialCount(materialCount);
}

const Material& SceneDatabase::GetMaterial(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetMaterial(index);
}

uint32_t SceneDatabase::GetMaterialCount() const
{
	return m_pSceneDatabaseImpl->GetMaterialCount();
}

///////////////////////////////////////////////////////////////////
// Texture
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddTexture(Texture texture)
{
	return m_pSceneDatabaseImpl->AddTexture(cd::MoveTemp(texture));
}

std::vector<Texture>& SceneDatabase::GetTextures()
{
	return m_pSceneDatabaseImpl->GetTextures();
}

const std::vector<Texture>& SceneDatabase::GetTextures() const
{
	return m_pSceneDatabaseImpl->GetTextures();
}

void SceneDatabase::SetTextureCount(uint32_t textureCount)
{
	return m_pSceneDatabaseImpl->SetTextureCount(textureCount);
}

const Texture& SceneDatabase::GetTexture(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetTexture(index);
}

uint32_t SceneDatabase::GetTextureCount() const
{
	return m_pSceneDatabaseImpl->GetTextureCount();
}

///////////////////////////////////////////////////////////////////
// Camera
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddCamera(Camera camera)
{
	return m_pSceneDatabaseImpl->AddCamera(cd::MoveTemp(camera));
}

std::vector<Camera>& SceneDatabase::GetCameras()
{
	return m_pSceneDatabaseImpl->GetCameras();
}

const std::vector<Camera>& SceneDatabase::GetCameras() const
{
	return m_pSceneDatabaseImpl->GetCameras();
}

void SceneDatabase::SetCameraCount(uint32_t cameraCount)
{
	return m_pSceneDatabaseImpl->SetCameraCount(cameraCount);
}

const Camera& SceneDatabase::GetCamera(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetCamera(index);
}

uint32_t SceneDatabase::GetCameraCount() const
{
	return m_pSceneDatabaseImpl->GetCameraCount();
}

///////////////////////////////////////////////////////////////////
// Light
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddLight(Light light)
{
	return m_pSceneDatabaseImpl->AddLight(cd::MoveTemp(light));
}

std::vector<Light>& SceneDatabase::GetLights()
{
	return m_pSceneDatabaseImpl->GetLights();
}

const std::vector<Light>& SceneDatabase::GetLights() const
{
	return m_pSceneDatabaseImpl->GetLights();
}

void SceneDatabase::SetLightCount(uint32_t lightCount)
{
	return m_pSceneDatabaseImpl->SetLightCount(lightCount);
}

const Light& SceneDatabase::GetLight(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetLight(index);
}

uint32_t SceneDatabase::GetLightCount() const
{
	return m_pSceneDatabaseImpl->GetLightCount();
}

///////////////////////////////////////////////////////////////////
// Bone
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddBone(Bone bone)
{
	return m_pSceneDatabaseImpl->AddBone(cd::MoveTemp(bone));
}

std::vector<Bone>& SceneDatabase::GetBones()
{
	return m_pSceneDatabaseImpl->GetBones();
}

const std::vector<Bone>& SceneDatabase::GetBones() const
{
	return m_pSceneDatabaseImpl->GetBones();
}

void SceneDatabase::SetBoneCount(uint32_t boneCount)
{
	return m_pSceneDatabaseImpl->SetBoneCount(boneCount);
}

const Bone& SceneDatabase::GetBone(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetBone(index);
}

const Bone* SceneDatabase::GetBoneByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetBoneByName(pName);
}

uint32_t SceneDatabase::GetBoneCount() const
{
	return m_pSceneDatabaseImpl->GetBoneCount();
}

///////////////////////////////////////////////////////////////////
// Animation
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddAnimation(Animation animation)
{
	return m_pSceneDatabaseImpl->AddAnimation(cd::MoveTemp(animation));
}

std::vector<Animation>& SceneDatabase::GetAnimations()
{
	return m_pSceneDatabaseImpl->GetAnimations();
}

const std::vector<Animation>& SceneDatabase::GetAnimations() const
{
	return m_pSceneDatabaseImpl->GetAnimations();
}

void SceneDatabase::SetAnimationCount(uint32_t animationCount)
{
	return m_pSceneDatabaseImpl->SetAnimationCount(animationCount);
}

const Animation& SceneDatabase::GetAnimation(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetAnimation(index);
}

uint32_t SceneDatabase::GetAnimationCount() const
{
	return m_pSceneDatabaseImpl->GetAnimationCount();
}

///////////////////////////////////////////////////////////////////
// Track
///////////////////////////////////////////////////////////////////
void SceneDatabase::AddTrack(Track track)
{
	return m_pSceneDatabaseImpl->AddTrack(cd::MoveTemp(track));
}

std::vector<Track>& SceneDatabase::GetTracks()
{
	return m_pSceneDatabaseImpl->GetTracks();
}

const std::vector<Track>& SceneDatabase::GetTracks() const
{
	return m_pSceneDatabaseImpl->GetTracks();
}

void SceneDatabase::SetTrackCount(uint32_t animationCount)
{
	return m_pSceneDatabaseImpl->SetTrackCount(animationCount);
}

const Track& SceneDatabase::GetTrack(uint32_t index) const
{
	return m_pSceneDatabaseImpl->GetTrack(index);
}

const Track* SceneDatabase::GetTrackByName(const char* pName) const
{
	return m_pSceneDatabaseImpl->GetTrackByName(pName);
}

uint32_t SceneDatabase::GetTrackCount() const
{
	return m_pSceneDatabaseImpl->GetTrackCount();
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void SceneDatabase::UpdateAABB()
{
	m_pSceneDatabaseImpl->UpdateAABB();
}

///////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////
SceneDatabase& SceneDatabase::operator<<(InputArchive& inputArchive)
{
	*m_pSceneDatabaseImpl << inputArchive;
	return *this;
}

SceneDatabase& SceneDatabase::operator<<(InputArchiveSwapBytes& inputArchive)
{
	*m_pSceneDatabaseImpl << inputArchive;
	return *this;
}

const SceneDatabase& SceneDatabase::operator>>(OutputArchive& outputArchive) const
{
	*m_pSceneDatabaseImpl >> outputArchive;
	return *this;
}

const SceneDatabase& SceneDatabase::operator>>(OutputArchiveSwapBytes& outputArchive) const
{
	*m_pSceneDatabaseImpl >> outputArchive;
	return *this;
}

}