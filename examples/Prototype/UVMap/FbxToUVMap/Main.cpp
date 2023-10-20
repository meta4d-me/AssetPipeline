#include "Framework/IConsumer.h"
#include "Framework/Processor.h"
#include "FbxProducer.h"
#include "Utilities/PerformanceProfiler.h"
#include "UVMapConsumer.hpp"

#include <filesystem>

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : input file path
	// argv[2] : output file path
	if (argc != 3)
	{
		return 1;
	}

	using namespace cdtools;

	PerformanceProfiler profiler("AssetPipeline");

	const char* pInputFilePath = argv[1];
	const char* pOutputFilePath = argv[2];

	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();
	FbxProducer producer(pInputFilePath);
	Processor processor(&producer, nullptr, pSceneDatabase.get());
	processor.Run();

	{
		// Export all meshes into one uvmap per set.
		uint32_t uvSetMaxCount = 0U;
		for (auto& mesh : pSceneDatabase->GetMeshes())
		{
			uvSetMaxCount = std::max(uvSetMaxCount, mesh.GetVertexUVSetCount());
		}

		for (uint32_t uvSetIndex = 0U; uvSetIndex < uvSetMaxCount; ++uvSetIndex)
		{
			std::stringstream ss;
			ss << pOutputFilePath << "/" << "UVMapTest" << "_UVSet" << uvSetIndex << ".png";
			UVMapConsumer consumer(ss.str().c_str());
			consumer.SetUVSetIndex(uvSetIndex);
			consumer.SetUVMapUnitSize(512, 512);
			consumer.SetUVMapMaxSize(2048, 2048);
			Processor processor(nullptr, &consumer, pSceneDatabase.get());
			processor.Run();
		}
	}

	{
		// Export per mesh into one uvmap per set.
		for (auto& mesh : pSceneDatabase->GetMeshes())
		{
			auto pMeshSceneDatabase = std::make_unique<cd::SceneDatabase>();
			mesh.SetID(cd::MeshID(0U));
			pMeshSceneDatabase->AddMesh(cd::MoveTemp(mesh));

			auto& targetMesh = pMeshSceneDatabase->GetMesh(0U);
			for (uint32_t uvSetIndex = 0U; uvSetIndex < targetMesh.GetVertexUVSetCount(); ++uvSetIndex)
			{
				std::stringstream ss;
				ss << pOutputFilePath << "/" << targetMesh.GetName() << "_UVSet" << uvSetIndex << ".png";
				UVMapConsumer consumer(ss.str().c_str());
				consumer.SetUVSetIndex(uvSetIndex);
				consumer.SetUVMapUnitSize(512, 512);
				consumer.SetUVMapMaxSize(2048, 2048);
				Processor processor(nullptr, &consumer, pMeshSceneDatabase.get());
				processor.Run();
			}
		}
	}

	return 0;
}