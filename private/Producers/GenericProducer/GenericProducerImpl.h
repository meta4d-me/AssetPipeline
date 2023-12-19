#pragma once

#include "Base/BitFlags.h"
#include "Base/Template.h"
#include "Producers/GenericProducer/GenericProducerOptions.h"
#include "Scene/ObjectIDGenerator.h"

#include <cstdint>
#include <map>
#include <string>

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
	explicit GenericProducerImpl(std::string filePath);
	GenericProducerImpl(const GenericProducerImpl&) = delete;
	GenericProducerImpl& operator=(const GenericProducerImpl&) = delete;
	GenericProducerImpl(GenericProducerImpl&&) = delete;
	GenericProducerImpl& operator=(GenericProducerImpl&&) = delete;
	~GenericProducerImpl() = default;

	void Execute(cd::SceneDatabase* pSceneDatabase);

	cd::BitFlags<GenericProducerOptions>& GetOptions() { return m_options; }
	const cd::BitFlags<GenericProducerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(GenericProducerOptions option) const { return m_options.IsEnabled(option); }

private:
	uint32_t GetImportFlags() const;

	void AddScene(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);
	void AddNodeRecursively(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene, const aiNode* pSourceNode, uint32_t nodeID);
	cd::MeshID AddMesh(cd::SceneDatabase* pSceneDatabase, const aiMesh* pSourceMesh);
	cd::MaterialID AddMaterial(cd::SceneDatabase* pSceneDatabase, const aiMaterial* pSourceMaterial);
	void AddMaterials(cd::SceneDatabase* pSceneDatabase, const aiScene* pSourceScene);

private:
	std::string m_filePath;
	std::string m_folderPath;
	cd::BitFlags<GenericProducerOptions> m_options;

	// Generate IDs for different scene objects
	cd::ObjectIDGenerator<cd::NodeID> m_nodeIDGenerator;
	cd::ObjectIDGenerator<cd::MeshID> m_meshIDGenerator;
	cd::ObjectIDGenerator<cd::MaterialID> m_materialIDGenerator;
	cd::ObjectIDGenerator<cd::TextureID> m_textureIDGenerator;

	std::map<const aiNode*, uint32_t> m_aiNodeToNodeIDLookup;
};

}