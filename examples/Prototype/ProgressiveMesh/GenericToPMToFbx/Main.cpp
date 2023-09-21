#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "GenericProducer.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"
#include "Utilities/PerformanceProfiler.h"

#include "progmesh.c"

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
		producer.ActivateTriangulateService();
		producer.ActivateTangentsSpaceService();
		producer.ActivateBoundingBoxService();
		producer.ActivateFlattenHierarchyService();
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

		std::vector<uint32_t> permutation;
		std::vector<uint32_t> map;
		permutation.resize(vertexCount);
		map.resize(vertexCount);
		constexpr uint32_t positionStride = cd::Point::Size * sizeof(cd::Point::ValueType);
		ProgressiveMesh(static_cast<int>(vertexCount), positionStride, reinterpret_cast<const float*>(mesh.GetVertexPositions().data()),
			static_cast<int>(polygonCount), reinterpret_cast<const int*>(indexBuffer.data()),
			reinterpret_cast<int*>(map.data()), reinterpret_cast<int*>(permutation.data()));

		std::vector<cd::Point> tempPositions = mesh.GetVertexPositions();
		for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
		{
			mesh.SetVertexPosition(permutation[vertexIndex], tempPositions[vertexIndex]);
		}

		for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
		{
			auto& polygon = mesh.GetPolygon(polygonIndex);
			assert(polygon.size() == 3U && "Need to triangulate.");

			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygon.size(); ++polygonVertexIndex)
			{
				uint32_t oldIndex = polygon[polygonVertexIndex].Data();
				uint32_t newIndex = permutation[oldIndex];
				polygon[polygonVertexIndex] = newIndex;
			}
		}
	}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}