#include "FbxConsumer.h"
#include "Framework/Processor.h"
#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "HalfEdgeMesh/Vertex.h"
#include "Scene/SceneDatabase.h"
#include "Utilities/PerformanceProfiler.h"

int main(int argc, char** argv)
{
	// argv[0] : exe name
	// argv[1] : output file path
	if (argc != 2)
	{
		return 1;
	}

	const char* pOutputFilePath = argv[1];
	
	using namespace cdtools;
	PerformanceProfiler profiler("HEMToFbx");
	
	auto pSceneDatabase = std::make_unique<cd::SceneDatabase>();

	// Generate halfedge mesh and convert to index mesh.
	{
		using namespace cd::hem;
		HalfEdgeMesh halfEdgeMesh;
		auto v0 = halfEdgeMesh.AddVertex();
		auto v1 = halfEdgeMesh.AddVertex();
		auto v2 = halfEdgeMesh.AddVertex();
		auto v3 = halfEdgeMesh.AddVertex();

		v0->SetPosition(cd::Point(0.0f, 1.0f, 0.0f));
		v1->SetPosition(cd::Point(0.0f, 0.0f, 1.0f));
		v2->SetPosition(cd::Point(-1.0f, 0.0f, -1.0f));
		v3->SetPosition(cd::Point(1.0f, 0.0f, -1.0f));

		auto e0 = halfEdgeMesh.AddEdge(v0, v1);
		auto e1 = halfEdgeMesh.AddEdge(v1, v2);
		auto e2 = halfEdgeMesh.AddEdge(v2, v0);
		auto e3 = halfEdgeMesh.AddEdge(v2, v3);
		auto e4 = halfEdgeMesh.AddEdge(v3, v0);
		auto e5 = halfEdgeMesh.AddEdge(v3, v1);

		auto f0 = halfEdgeMesh.AddFace({ e0->GetHalfEdge(), e1->GetHalfEdge(), e2->GetHalfEdge() });
		auto f1 = halfEdgeMesh.AddFace({ e2->GetHalfEdge()->GetTwin(), e3->GetHalfEdge(), e4->GetHalfEdge() });
		auto f2 = halfEdgeMesh.AddFace({ e4->GetHalfEdge()->GetTwin(), e5->GetHalfEdge(), e0->GetHalfEdge()->GetTwin() });
		auto f3 = halfEdgeMesh.AddFace({ e1->GetHalfEdge()->GetTwin(), e5->GetHalfEdge()->GetTwin(), e3->GetHalfEdge()->GetTwin() });
		f3.value()->SetIsBoundary(true);
		halfEdgeMesh.Dump();

		auto mesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::TopologyFirst);
		mesh.SetID(cd::MeshID(0U));
		pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
	}

	// Export.
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.SetDumpSceneDatabaseEnable(true);
		processor.Run();
	}

	return 0;
}