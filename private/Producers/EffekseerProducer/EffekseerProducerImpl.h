#pragma once

#include "Base/Template.h"
#include "Scene/SceneDatabase.h"

namespace cdtools
{

class EffekseerProducerImpl final
{
public:
	EffekseerProducerImpl() = delete;
	explicit EffekseerProducerImpl(std::string filePath);
	EffekseerProducerImpl(const EffekseerProducerImpl&) = delete;
	EffekseerProducerImpl& operator=(const EffekseerProducerImpl&) = delete;
	EffekseerProducerImpl(EffekseerProducerImpl&&) = delete;
	EffekseerProducerImpl& operator=(EffekseerProducerImpl&&) = delete;
	~EffekseerProducerImpl() = default;

	void Execute(cd::SceneDatabase* pSceneDatabase);
};

}