#pragma once

#include "Framework/IProducer.h"
#include "Producers/GenericProducer/GenericProducerOptions.h"

#include <cstdint>

struct aiMaterial;
struct aiMesh;
struct aiNode;

namespace cdtools
{

class GenericProducerImpl;

// GenericProducer can be used to import all kinds of model formats, such as fbx, glTF, obj. Powered by assimp.
class TOOL_API GenericProducer final : public IProducer
{
public:
	GenericProducer() = delete;
	explicit GenericProducer(const char* pFilePath);
	GenericProducer(const GenericProducer&) = delete;
	GenericProducer& operator=(const GenericProducer&) = delete;
	GenericProducer(GenericProducer&&) = delete;
	GenericProducer& operator=(GenericProducer&&) = delete;
	virtual ~GenericProducer();

	void SetSceneDatabaseIDs(uint32_t nodeID, uint32_t meshID, uint32_t materialID, uint32_t textureID, uint32_t lightID);
	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	void EnableOption(GenericProducerOptions option);
	void DisableOption(GenericProducerOptions option);
	bool IsOptionEnabled(GenericProducerOptions option) const;

private:
	GenericProducerImpl* m_pGenericProducerImpl;
};

}