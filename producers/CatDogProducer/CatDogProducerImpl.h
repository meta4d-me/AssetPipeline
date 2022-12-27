#pragma once

#include "Base/Template.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CatDogProducerImpl final
{
public:
	CatDogProducerImpl() = delete;
	explicit CatDogProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	CatDogProducerImpl(const CatDogProducerImpl&) = delete;
	CatDogProducerImpl& operator=(const CatDogProducerImpl&) = delete;
	CatDogProducerImpl(CatDogProducerImpl&&) = delete;
	CatDogProducerImpl& operator=(CatDogProducerImpl&&) = delete;
	~CatDogProducerImpl() = default;
	void Execute(cd::SceneDatabase* pSceneDatabase);

private:
	std::string m_filePath;
};

}