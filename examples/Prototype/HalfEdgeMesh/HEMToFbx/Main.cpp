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
		using namespace cd;
		using namespace cd::hem;
		HalfEdgeMesh halfEdgeMesh;
		auto v0 = halfEdgeMesh.AddVertex();
		auto v1 = halfEdgeMesh.AddVertex();
		auto v2 = halfEdgeMesh.AddVertex();
		auto v3 = halfEdgeMesh.AddVertex();
	
		v0->SetPosition(cd::Point(0.0f, 0.0f, 1.0f));
		v1->SetPosition(cd::Point(-1.0f, 0.0f, 0.0f));
		v2->SetPosition(cd::Point(0.0f, 0.0f, -1.0f));
		v3->SetPosition(cd::Point(1.0f, 0.0f, 0.0f));
		
		auto e01 = halfEdgeMesh.AddEdge(v0, v1);
		auto e12 = halfEdgeMesh.AddEdge(v1, v2);
		auto e20 = halfEdgeMesh.AddEdge(v2, v0);
		auto e23 = halfEdgeMesh.AddEdge(v2, v3);
		auto e30 = halfEdgeMesh.AddEdge(v3, v0);
	
		auto f0 = halfEdgeMesh.AddFace({ e01->GetHalfEdge(), e12->GetHalfEdge(), e20->GetHalfEdge() });
		auto f1 = halfEdgeMesh.AddFace({ e20->GetHalfEdge()->GetTwin(), e23->GetHalfEdge(), e30->GetHalfEdge() });
		auto f2 = halfEdgeMesh.AddFace({ e30->GetHalfEdge()->GetTwin(), e23->GetHalfEdge()->GetTwin(),
			e12->GetHalfEdge()->GetTwin(), e01->GetHalfEdge()->GetTwin() });
		f2.value()->SetIsBoundary(true);
		// Split edges to generate more vertices.
		// Before:
		//          v0
		//		   /  \
		//       v1    v3
		//         \  /
		//          v2
		// 
		// After:
		//          v0
		//		   / | \
		//       v1-v4-v3
		//         \ | /
		//          v2
		//
		// After:
		// v0 -> v5 -> v2 is hard to draw.
		//           v0
		//		   //| \
		//		  / ||  \
		//		 / / |   \
		//		/ /  |    \
		//	   /  |  |     \
		//   v1--v5--v4-----v3
		//     \  |  |     /
		//		\ \  |    /
		//		 \ | |   /
		//        \ \|  /
		//         \\| /
		//           v2
		auto optV4 = halfEdgeMesh.SplitEdge(e20);
		auto v4 = optV4.value();
		auto v4v1 = v4->GetHalfEdgeToVertex(v1).value();
		auto v5 = halfEdgeMesh.SplitEdge(v4v1->GetEdge()).value();
		halfEdgeMesh.Dump();

		{
			auto mesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::TopologyFirst);
			mesh.SetName("orgin");
			mesh.SetID(cd::MeshID(0U));
			pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
		}

		// Collapse edge v5v2.
		// So v2 and v5 are removed. New v6 is created. so 
		// After:
		// v0 -> v6 and v3 -> v6 are hard to draw.
		//           v0
		//		   //| \
		//		  / ||  \
		//		 / / |   \
		//		/  | |    \
		//	   /  /  |     \
		//   v1   | v4-----v3
		//     \  / /    /
		//		\ |/  /
		//		  v6
		//
		auto v5v2 = v5->GetHalfEdgeToVertex(v2).value();
		halfEdgeMesh.CollapseEdge(v5v2->GetEdge());
		halfEdgeMesh.Dump();

		{
			auto mesh = cd::Mesh::FromHalfEdgeMesh(halfEdgeMesh, cd::ConvertStrategy::TopologyFirst);
			mesh.SetName("collapsed");
			mesh.SetID(cd::MeshID(1U));
			pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
		}
	}

	// Export.
	{
		FbxConsumer consumer(pOutputFilePath);
		Processor processor(nullptr, &consumer, pSceneDatabase.get());
		processor.Run();
	}

	return 0;
}