#pragma once

#include "Framework/IProducer.h"
#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Math/MeshGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"

#include <string>

namespace cdtools
{

class ProgressiveMeshProducer final : public IProducer
{
public:
	ProgressiveMeshProducer() = delete;
	explicit ProgressiveMeshProducer(const char* pFilePath) : m_filePath(pFilePath) {}
	ProgressiveMeshProducer(const ProgressiveMeshProducer&) = delete;
	ProgressiveMeshProducer& operator=(const ProgressiveMeshProducer&) = delete;
	ProgressiveMeshProducer(ProgressiveMeshProducer&&) = delete;
	ProgressiveMeshProducer& operator=(ProgressiveMeshProducer&&) = delete;
	virtual ~ProgressiveMeshProducer() {}

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override
	{
		pSceneDatabase->SetName("ProgressiveMeshProducer");
	}

private:
	std::string m_filePath;
};

}