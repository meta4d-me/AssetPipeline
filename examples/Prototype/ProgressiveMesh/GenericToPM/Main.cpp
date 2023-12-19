#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "ProgressiveMesh/ProgressiveMesh.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if (argc != 3)
	{
		return 1;
	}

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];
	
	using namespace cdtools;
	PerformanceProfiler profiler("ProgressiveMesh");
	
	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	// Import
	{
		GenericProducer producer(pInputFilePath);
		producer.EnableOption(GenericProducerOptions::FlattenTransformHierarchy);

		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(false);
		processor.Run();
	}
	
	// Processing
	for (auto& mesh : pSceneDatabase->GetMeshes())
	{
		uint32_t vertexCount = mesh.GetVertexCount();
		uint32_t polygonCount = mesh.GetPolygonCount();

		std::vector<uint32_t> indexBuffer;
		for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
		{
			const auto& polygon = mesh.GetPolygon(polygonIndex);
			assert(polygon.size() == 3U && "Need to triangulate.");

			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygon.size(); ++polygonVertexIndex)
			{
				indexBuffer.push_back(polygon[polygonVertexIndex].Data());
			}
		}

		//auto hem = cd::HalfEdgeMesh::FromIndexedMesh(mesh);
		//auto boundaryMesh = cd::Mesh::FromHalfEdgeMesh(hem, cd::ConvertStrategy::BoundaryOnly);
		auto pm = cd::ProgressiveMesh::FromIndexedMesh(mesh);
		//pm.InitBoundary(boundaryMesh);
		auto [permutation, map] = pm.BuildCollapseOperations();
	}

	return 0;
}