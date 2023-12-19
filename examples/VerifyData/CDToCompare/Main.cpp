#include "CDProducer.h"
#include "Framework/Processor.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input1 file path
	// argv[2] : input2 file path
	if (argc != 3)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("AssetPipeline");

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	const char* pInput1FilePath = argv[1];
	const char* pInput2FilePath = argv[2];

	{
		cd::SceneDatabase newSceneDatabase;
		CDProducer producer(pInput1FilePath);
		Processor processor(&producer, nullptr, &newSceneDatabase);
		processor.Run();

		pSceneDatabase->Merge(cd::MoveTemp(newSceneDatabase));
	}

	{
		cd::SceneDatabase newSceneDatabase;
		CDProducer producer(pInput2FilePath);
		Processor processor(&producer, nullptr, &newSceneDatabase);
		processor.Run();

		pSceneDatabase->Merge(cd::MoveTemp(newSceneDatabase));
	}

	const auto& mesh1 = pSceneDatabase->GetMesh(0);
	const auto& mesh2 = pSceneDatabase->GetMesh(1);

	assert(mesh1.GetVertexCount() == mesh2.GetVertexCount());
	assert(mesh1.GetPolygonCount() == mesh2.GetPolygonCount());

	for (uint32_t vertexIndex = 0U; vertexIndex < mesh1.GetVertexCount(); ++vertexIndex)
	{
		assert(mesh1.GetVertexPosition(vertexIndex) == mesh2.GetVertexPosition(vertexIndex));
		assert(mesh1.GetVertexNormal(vertexIndex) == mesh2.GetVertexNormal(vertexIndex));
		const auto& t1 = mesh1.GetVertexTangent(vertexIndex);
		const auto& t2 = mesh2.GetVertexTangent(vertexIndex);
		assert(t1 == t2);
		//assert(mesh1.GetVertexBiTangent(vertexIndex) == mesh2.GetVertexBiTangent(vertexIndex));
		assert(mesh1.GetVertexUV(0, vertexIndex) == mesh2.GetVertexUV(0, vertexIndex));
	}

	for (uint32_t polygonIndex = 0U; polygonIndex < mesh1.GetPolygonCount(); ++polygonIndex)
	{
		const auto& p1 = mesh1.GetPolygon(polygonIndex);
		const auto& p2 = mesh2.GetPolygon(polygonIndex);
		assert(p1.size() == p2.size());
		assert(p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]);
	}

	return 0;
}