#pragma once

#include "Framework/IProducer.h"
#include "Producers/GenericProducer/GenericProducerOptions.h"

namespace cdtools
{

class GenericProducerImpl;

//
// GenericProducer can be used to import static meshes with materials from all kinds of model formats, such as fbx, glTF, obj.
// Powered by assimp. Note that GenericProducer doesn't support Light because aiLight data structure is too old to convert to correct intensity/color/...
// Animation related data is also limited because assimp is too slow to give it a fix.
// If you need advanced Light/Skeleton Animation/Blendshape/... data, please use FbxProducer.
//
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

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	void EnableOption(GenericProducerOptions option);
	void DisableOption(GenericProducerOptions option);
	bool IsOptionEnabled(GenericProducerOptions option) const;

private:
	GenericProducerImpl* m_pGenericProducerImpl;
};

}