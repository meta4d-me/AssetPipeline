#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
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
		processor.Run();
	}
	
	// Processing
	uint32_t meshCount = pSceneDatabase->GetMeshCount();
	for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
	{
		const auto& mesh = pSceneDatabase->GetMesh(meshIndex);
		uint32_t vertexCount = mesh.GetVertexCount();
		uint32_t polygonCount = mesh.GetPolygonCount();

		std::vector<uint32_t> indexBuffer;
		indexBuffer.reserve(polygonCount);
		for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
		{
			const auto& polygon = mesh.GetPolygon(polygonIndex);
			assert(polygon.size() == 3U && "Need to triangulate.");

			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygon.size(); ++polygonVertexIndex)
			{
				indexBuffer.push_back(polygon[polygonVertexIndex].Data());
			}
		}

		auto pm = cd::ProgressiveMesh::FromIndexedMesh(mesh);
		for (uint32_t otherMeshIndex = 0U; otherMeshIndex < meshCount; ++otherMeshIndex)
		{
			if (meshIndex == otherMeshIndex)
			{
				continue;
			}

			const auto& otherMesh = pSceneDatabase->GetMesh(otherMeshIndex);
			if (!mesh.GetAABB().Intersects(otherMesh.GetAABB()))
			{
				continue;
			}

			cd::AABB intersection = mesh.GetAABB().GetIntersection(otherMesh.GetAABB());
			pm.InitBoundary(intersection);
		}

		auto lodMesh = pm.GenerateLodMesh(0.1f, 2000, &mesh);
		lodMesh.SetName(std::format("{}_reduced", mesh.GetName()).c_str());
		lodMesh.SetID(pSceneDatabase->GetMeshCount());
		pSceneDatabase->AddMesh(cd::MoveTemp(lodMesh));
	}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}