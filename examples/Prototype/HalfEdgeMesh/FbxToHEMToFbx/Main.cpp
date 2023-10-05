#include "FbxConsumer.h"
#include "FbxProducer.h"
#include "Framework/Processor.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "Hashers/HashCombine.hpp"
#include "Scene/SceneDatabase.h"
#include "Utilities/PerformanceProfiler.h"

namespace
{

cd::Mesh GenerateBoundaryMesh(const cd::Mesh& mesh)
{
	uint32_t newVertexIndex = 0U;
	std::map<uint32_t, uint32_t> mapPosToVertexIndex;
	std::map<uint32_t, uint32_t> mapOldIndexToNewIndex;
	for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
	{
		const auto& position = mesh.GetVertexPosition(vertexIndex);
		uint32_t positionHash = cd::HashCombine(cd::Math::CastFloatToU32(position.x()),
			cd::HashCombine(cd::Math::CastFloatToU32(position.y()), cd::Math::CastFloatToU32(position.z())));

		auto itVertexIndex = mapPosToVertexIndex.find(positionHash);
		if (itVertexIndex != mapPosToVertexIndex.end())
		{
			uint32_t uniqueVertexIndex = itVertexIndex->second;
			mapOldIndexToNewIndex[vertexIndex] = mapOldIndexToNewIndex[uniqueVertexIndex];
		}
		else
		{
			mapPosToVertexIndex[positionHash] = vertexIndex;
			mapOldIndexToNewIndex[vertexIndex] = newVertexIndex++;
		}
	}

	uint32_t newVertexCount = static_cast<uint32_t>(mapPosToVertexIndex.size());
	uint32_t newPolygonCount = mesh.GetPolygonCount();
	cd::Mesh newMesh(newVertexCount, newPolygonCount);
	newMesh.SetName(mesh.GetName());
	newMesh.SetMaterialID(mesh.GetMaterialID());

	for (const auto& [oldIndex, newIndex] : mapOldIndexToNewIndex)
	{
		newMesh.SetVertexPosition(newIndex, mesh.GetVertexPosition(oldIndex));
	}

	for (uint32_t polygonIndex = 0U; polygonIndex < newPolygonCount; ++polygonIndex)
	{
		const auto& oldPolygon = mesh.GetPolygon(polygonIndex);
		auto& newPolygon = newMesh.GetPolygon(polygonIndex);
		for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < oldPolygon.size(); ++polygonVertexIndex)
		{
			uint32_t oldIndex = oldPolygon[polygonVertexIndex].Data();
			newPolygon.push_back(mapOldIndexToNewIndex[oldIndex]);
		}
	}

	return newMesh;
}

}

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
	auto pNewSceneDatabase = std::make_unique<cd::SceneDatabase>();

	// Import
	{
		FbxProducer producer(pInputFilePath);
		producer.SetWantTriangulate(false);
		Processor processor(&producer, nullptr, pSceneDatabase.get());
		processor.Run();
	}

	// Processing
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		cd::Mesh boundaryMesh = GenerateBoundaryMesh(mesh);
		auto halfEdgeMesh = cd::HalfEdgeMesh::FromIndexedMesh(boundaryMesh);
		assert(halfEdgeMesh.IsValid());

		auto newMesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::BoundaryOnly);
		newMesh.SetName(mesh.GetName());
		newMesh.SetID(pNewSceneDatabase->GetMeshCount());
		pNewSceneDatabase->AddMesh(cd::MoveTemp(newMesh));
	}

	// Export
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pNewSceneDatabase.get());
		processor.Run();
	}

	return 0;
}