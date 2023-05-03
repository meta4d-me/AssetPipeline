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
		pSceneDatabase->SetName("ProgressiveMesh");
		const cd::Mesh& mesh = pSceneDatabase->GetMesh(0);
		
		uint32_t vertexCount = mesh.GetVertexCount();
		for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
		{
			printf("\tVertex [%u]\n", vertexIndex);
			printf("\t\tAdjacent Vertex : ");
			for (cd::VertexID adjVertexID : mesh.GetVertexAdjacentVertexArray(vertexIndex))
			{
				printf("%u, ", adjVertexID.Data());
			}
			printf("\n");

			printf("\t\tAdjacent Polygon : ");
			for (cd::PolygonID adjPolygonID : mesh.GetVertexAdjacentPolygonArray(vertexIndex))
			{
				printf("%u, ", adjPolygonID.Data());
			}
			printf("\n");

			const cd::Point pos = mesh.GetVertexPosition(vertexIndex);
		}
	}

private:
	std::string m_filePath;
};

}