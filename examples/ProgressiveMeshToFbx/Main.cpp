#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "Utilities/PerformanceProfiler.h"

#include "MeshGenerteProducer.hpp"
#include "ProgressiveMeshProducer.hpp"

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

	// Generate a source mesh.
	{
		cd::VertexFormat vertexFormat;
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);

		//cd::Sphere shape(cd::Point(0.0f, 0.0f, 0.0f), 2.0f);
		//std::optional<cd::Mesh> optMesh = cd::MeshGenerator::Generate(shape, 20, 20, vertexFormat);
		cd::Box shape(cd::Vec3f(-1.0f, -1.0f, -1.0f), cd::Vec3f(1.0f, 1.0f, 1.0f));
		std::optional<cd::Mesh> optMesh = cd::MeshGenerator::Generate(shape, vertexFormat);
		assert(optMesh.has_value());
		cd::Mesh& mesh = optMesh.value();
		mesh.SetID(cd::MeshID(0U));
		pSceneDatabase->SetMeshCount(1U);
		pSceneDatabase->AddMesh(MoveTemp(mesh));

		Processor processor(nullptr, nullptr, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(false);
		processor.SetCalculateConnetivityDataEnable(true);
		processor.Run();
	}

	// Generate a progressive target mesh.
	{
		ProgressiveMeshProducer producer("");
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(&producer, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}