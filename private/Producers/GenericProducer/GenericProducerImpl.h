#pragma once

#include "Base/EnumOptions.hpp"
#include "Base/Template.h"
#include "Producers/GenericProducer/GenericProducerOptions.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <string>

struct aiAnimation;
struct aiLight;
struct aiMaterial;
struct aiMesh;
struct aiNode;
struct aiScene;

namespace cd
{

class Mesh;
class SceneDatabase;

}

namespace cdtools
{

class GenericProducerImpl final
{
public:
	GenericProducerImpl() = delete;
	explicit GenericProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	GenericProducerImpl(const GenericProducerImpl&) = delete;
	GenericProducerImpl& operator=(const GenericProducerImpl&) = delete;
	GenericProducerImpl(GenericProducerImpl&&) = delete;
	GenericProducerImpl& operator=(GenericProducerImpl&&) = delete;
	~GenericProducerImpl() = default;

	void SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID);
	void Execute(cd::SceneDatabase* pSceneDatabase);

	cd::EnumOptions<GenericProducerOptions>& GetOptions() { return m_options; }
	const cd::EnumOptions<GenericProducerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(GenericProducerOptions option) const { return m_options.IsEnabled(option); }

private:
	uint32_t GetImportFlags() const;

	// Import different type of objects in the SceneGraph.
	void AddScene(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);
	void AddNodeRecursively(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID);
	void AddLight(cd::SceneDatabase* pSceneDatabase, const aiLight* pSourceLight);
	void AddMaterials(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);
	cd::MaterialID AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	cd::MeshID AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);
	void AddMeshBones(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh, cd::Mesh& mesh);
	void AddAnimation(cd::SceneDatabase* pSceneDatabase, const aiAnimation* pSourceAnimation);

	// Post process stages.
	void RemoveBoneReferenceNodes(cd::SceneDatabase* pSceneDatabase);
	void KeepNodeIDAndIndexSame(cd::SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
	std::string m_folderPath;
	cd::EnumOptions<GenericProducerOptions> m_options;

	// Generate IDs for different objects
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;
	cd::ObjectIDGenerator<cd::LightID> m_lightIDGenerator;
	cd::ObjectIDGenerator<cd::BoneID> m_boneIDGenerator;
	cd::ObjectIDGenerator<cd::AnimationID> m_animationIDGenerator;
	cd::ObjectIDGenerator<cd::TrackID> m_trackIDGenerator;

	std::map<uint32_t, uint32_t> m_nodeIDToNodeIndexLookup;
	std::map<const aiNode*, uint32_t> m_aiNodeToNodeIDLookup;
};

}