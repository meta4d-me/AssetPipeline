#include "HalfEdgeMesh/HalfEdgeMesh.h"

#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "HalfEdgeMesh/Vertex.h"
#include "Scene/Mesh.h"

#include <map>
#include <unordered_map>
#include <unordered_set>

namespace
{

// Helper used to get set of pointers in a list (used by validate and describe):
template<typename T>
static std::unordered_set<const T*> ElementAddresses(const std::list<T>& list)
{
	std::unordered_set<const T*> addressSet;
	for (const auto& element : list)
	{
		auto result = addressSet.emplace(&element);
		assert(result.second);
	}
	return addressSet;
}

}

namespace cd::hem
{

HalfEdgeMesh::HalfEdgeMesh() = default;
HalfEdgeMesh::HalfEdgeMesh(HalfEdgeMesh&&) = default;
HalfEdgeMesh& HalfEdgeMesh::operator=(HalfEdgeMesh&&) = default;
HalfEdgeMesh::~HalfEdgeMesh() = default;

HalfEdgeMesh HalfEdgeMesh::FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons)
{
	HalfEdgeMesh halfEdgeMesh;

	// Init vertex data.
	std::vector<VertexRef> verticesLookUp;
	for (const auto& vertex : vertices)
	{
		verticesLookUp.emplace_back(halfEdgeMesh.EmplaceVertex());
		verticesLookUp.back()->SetPosition(vertex);
	}

	std::unordered_map<cd::EdgePair, HalfEdgeRef> halfEdgesLookUp;
	auto AddLoop = [&](const std::vector<cd::VertexID>& polygon, bool isBoundary)
	{
		// All faces must be non-degenerate.
		assert(polygon.size() >= 3U);

		HalfEdgeRef prev = halfEdgeMesh.GetHalfEdges().end();
		FaceRef face = halfEdgeMesh.EmplaceFace(isBoundary);
		for (uint32_t vertexIndex = 0U; vertexIndex < polygon.size(); ++vertexIndex)
		{
			cd::VertexID a = polygon[vertexIndex];
			cd::VertexID b = polygon[(vertexIndex + 1) % polygon.size()];
			assert(a != b);

			VertexRef va = verticesLookUp[a.Data()];
			VertexRef vb = verticesLookUp[b.Data()];

			HalfEdgeRef halfEdge = halfEdgeMesh.EmplaceHalfEdge();
			halfEdge->SetFace(face);
			// Face's HalfEdgeRef will point to the first edge.
			if (0U == vertexIndex)
			{
				face->SetHalfEdge(halfEdge);
			}

			halfEdge->SetVertex(va);
			if (va->GetHalfEdge() == halfEdgeMesh.GetHalfEdges().end())
			{
				// The first time to mention this vertex, it is time to init half edge pointer.
				assert(!isBoundary);
				va->SetHalfEdge(halfEdge);
			}

			auto result = halfEdgesLookUp.emplace(std::make_pair(a, b), halfEdge);
			// std::unorded_map::emplace will return a pair to present (data pointer, isFirstTimeInsert).
			// Make sure that it is the first time to insert (a, b) pair.
			// Or the input mesh is not an oriented, manifold mesh.
			assert(result.second);

			auto itTwin = halfEdgesLookUp.find(std::make_pair(b, a));
			if (itTwin == halfEdgesLookUp.end())
			{
				// The first time to mention twin half edge, it is time to init edge.
				assert(!isBoundary);
				EdgeRef edge = halfEdgeMesh.EmplaceEdge();
				edge->SetHalfEdge(halfEdge);
				halfEdge->SetEdge(edge);
			}
			else
			{
				// Twin half edge already initialized, build referecens with current half edge.
				HalfEdgeRef twin = itTwin->second;
				assert(twin->GetTwin() == halfEdgeMesh.GetHalfEdges().end());
				twin->SetTwin(halfEdge);
				halfEdge->SetTwin(twin);
				halfEdge->SetEdge(twin->GetEdge());
			}

			if (vertexIndex != 0)
			{
				HalfEdge::SetNextAndPrev(prev, halfEdge);
			}
			prev = halfEdge;
		}

		// Connect last half edge to first half edge to be a loop.
		HalfEdge::SetNextAndPrev(prev, face->GetHalfEdge());
	};

	// Init non-boundary loops.
	for (const auto& polygon : polygons)
	{
		AddLoop(polygon, false);
	}

	// Find half edges which are at a boundary. These half edges should miss twin half edge.
	std::map<cd::VertexID, cd::VertexID> nextOnBoundary;
	for (const auto& [edge, halfEdge] : halfEdgesLookUp)
	{
		if (halfEdge->GetTwin() == halfEdgeMesh.GetHalfEdges().end())
		{
			auto result = nextOnBoundary.emplace(edge.second, edge.first);
			assert(result.second);
		}
	}

	// Init boundary loops.
	while (!nextOnBoundary.empty())
	{
		std::vector<cd::VertexID> boundaryLoop;
		boundaryLoop.emplace_back(nextOnBoundary.begin()->first);

		do
		{
			auto next = nextOnBoundary.find(boundaryLoop.back());
			assert(next != nextOnBoundary.end());

			boundaryLoop.emplace_back(next->second);

			nextOnBoundary.erase(next);
		} while (boundaryLoop[0] != boundaryLoop.back());
		boundaryLoop.pop_back();

		AddLoop(boundaryLoop, true);
	}

	return halfEdgeMesh;
}

HalfEdgeMesh HalfEdgeMesh::FromIndexedMesh(const cd::Mesh& mesh)
{
	auto halfEdgeMesh = HalfEdgeMesh::FromIndexedFaces(mesh.GetVertexPositions(), mesh.GetPolygons());

	// Fill corner uv/normal data.
	uint32_t vertexIndex = 0U;
	for (const auto& vertex : halfEdgeMesh.GetVertices())
	{
		assert(vertexIndex < mesh.GetVertexCount());
		HalfEdgeRef h = vertex.GetHalfEdge();
	
		do
		{
			if (!h->GetFace()->IsBoundary())
			{
				// Only copy base uv.
				h->SetCornerUV(mesh.GetVertexUV(0U, vertexIndex));
				h->SetCornerNormal(mesh.GetVertexNormal(vertexIndex));
			}
	
			h = h->GetRotateNext();
		} while (h != vertex.GetHalfEdge());
	
		++vertexIndex;
	}
	assert(vertexIndex == mesh.GetVertexCount());

	return halfEdgeMesh;
}

bool HalfEdgeMesh::IsValid() const
{
	for (const auto& vertex : GetVertices())
	{
		if (!vertex.IsValid())
		{
			return false;
		}
	}

	for (const auto& edge : GetEdges())
	{
		if (!edge.IsValid())
		{
			return false;
		}
	}

	for (const auto& face : GetFaces())
	{
		if (!face.IsValid())
		{
			return false;
		}
	}

	for (const auto& halfEdge : GetHalfEdges())
	{
		if (!halfEdge.IsValid())
		{
			return false;
		}
	}

	auto inVertices = ElementAddresses(GetVertices());
	auto inEdges = ElementAddresses(GetEdges());
	auto inFaces = ElementAddresses(GetFaces());
	auto inHalfEdges = ElementAddresses(GetHalfEdges());

	for (const auto& vertex : GetVertices())
	{
		if (!inHalfEdges.count(&*vertex.GetHalfEdge()))
		{
			return false;
		}
	}

	for (const auto& edge : GetEdges())
	{
		if (!inHalfEdges.count(&*edge.GetHalfEdge()))
		{
			return false;
		}
	}

	for (const auto& face : GetFaces())
	{
		if (!inHalfEdges.count(&*face.GetHalfEdge()))
		{
			return false;
		}
	}

	for (const auto& halfEdge : GetHalfEdges())
	{
		if (!inVertices.count(&*halfEdge.GetVertex()))
		{
			return false;
		}

		if (!inEdges.count(&*halfEdge.GetEdge()))
		{
			return false;
		}

		if (!inFaces.count(&*halfEdge.GetFace()))
		{
			return false;
		}

		if (!inHalfEdges.count(&*halfEdge.GetTwin()))
		{
			return false;
		}

		if (!inHalfEdges.count(&*halfEdge.GetPrev()))
		{
			return false;
		}

		if (!inHalfEdges.count(&*halfEdge.GetNext()))
		{
			return false;
		}
	}

	std::unordered_map<VertexCRef, std::unordered_set<HalfEdgeCRef>> vertexHalfEdges;
	std::unordered_map<EdgeCRef, std::unordered_set<HalfEdgeCRef>> edgeHalfEdges;
	std::unordered_map<FaceCRef, std::unordered_set<HalfEdgeCRef>> faceHalfEdges;
	for (HalfEdgeCRef h = GetHalfEdges().begin(); h != GetHalfEdges().end(); ++h)
	{
		VertexCRef v = h->GetVertex();
		auto vresult = vertexHalfEdges[v].emplace(h);
		assert(vresult.second);

		EdgeCRef e = h->GetEdge();
		auto eresult = edgeHalfEdges[e].emplace(h);
		assert(eresult.second);

		FaceCRef f = h->GetFace();
		auto fresult = faceHalfEdges[f].emplace(h);
		assert(fresult.second);
	}

	for (VertexCRef v = GetVertices().begin(); v != GetVertices().end(); ++v)
	{
		auto toVisit = vertexHalfEdges[v];
		HalfEdgeCRef h = v->GetHalfEdge();
		do
		{
			if (h->GetVertex() != v)
			{
				return false;
			}

			auto itHalfEdge = toVisit.find(h);
			if (itHalfEdge == toVisit.end())
			{
				return false;
			}
			toVisit.erase(itHalfEdge);

			h = h->GetRotateNext();
		} while (h != v->GetHalfEdge());

		if (!toVisit.empty())
		{
			return false;
		}

		if (vertexHalfEdges[v].size() < 2)
		{
			return false;
		}
	}

	for (EdgeCRef e = GetEdges().begin(); e != GetEdges().end(); ++e)
	{
		auto toVisit = edgeHalfEdges[e];
		HalfEdgeCRef h = e->GetHalfEdge();
		do
		{
			if (h->GetEdge() != e)
			{
				return false;
			}

			auto itHalfEdge = toVisit.find(h);
			if (itHalfEdge == toVisit.end())
			{
				return false;
			}
			toVisit.erase(itHalfEdge);

			h = h->GetTwin();
		} while (h != e->GetHalfEdge());

		if (!toVisit.empty())
		{
			return false;
		}

		if (edgeHalfEdges[e].size() != 2)
		{
			return false;
		}
	}

	for (FaceCRef f = GetFaces().begin(); f != GetFaces().end(); ++f)
	{
		auto toVisit = faceHalfEdges[f];
		HalfEdgeCRef h = f->GetHalfEdge();
		do
		{
			if (h->GetFace() != f)
			{
				return false;
			}

			auto itHalfEdge = toVisit.find(h);
			if (itHalfEdge == toVisit.end())
			{
				return false;
			}
			toVisit.erase(itHalfEdge);

			h = h->GetNext();
		} while (h != f->GetHalfEdge());

		if (!toVisit.empty())
		{
			return false;
		}

		if (faceHalfEdges[f].size() < 3)
		{
			return false;
		}
	}

	return true;
}

void HalfEdgeMesh::Dump() const
{
	printf("\nHalfEdgeMesh : \n");
	printf("\tVertexCount : %llu\n", GetVertices().size());
	printf("\tHalfEdgeCount : %llu\n", GetHalfEdges().size());
	printf("\tEdgeCount : %llu\n", GetEdges().size());
	printf("\tFaceCount : %llu\n", GetFaces().size());
	printf("\n");

	for (const auto& vertex : GetVertices())
	{
		printf("[Vertex %d]\n", vertex.GetID().Data());
		printf("\tIsOnBoundary : %d\n", vertex.IsOnBoundary());
		printf("\tPosition : (%f, %f, %f)\n", vertex.GetPosition().x(), vertex.GetPosition().y(), vertex.GetPosition().z());
		printf("\t[Associated HalfEdge %d]\n", vertex.GetHalfEdge()->GetID().Data());
	}
	printf("\n");

	for (const auto& halfEdge : GetHalfEdges())
	{
		printf("[HalfEdge %d]\n", halfEdge.GetID().Data());
		printf("\tTwin : [HalfEdge %d], Prev : [HalfEdge %d], Next : [HalfEdge %d]\n", halfEdge.GetTwin()->GetID().Data(),
			halfEdge.GetPrev()->GetID().Data(), halfEdge.GetNext()->GetID().Data());
		printf("\tv%d -> v%d\n", halfEdge.GetVertex()->GetID().Data(), halfEdge.GetEndVertex()->GetID().Data());
		printf("\t[Associated Vertex %d]\n", halfEdge.GetVertex()->GetID().Data());
		printf("\t[Associated Edge %d]\n", halfEdge.GetEdge()->GetID().Data());
		printf("\t[Associated Face %d]\n", halfEdge.GetFace()->GetID().Data());
	}
	printf("\n");

	for (const auto& edge : GetEdges())
	{
		printf("[Edge %d]\n", edge.GetID().Data());
		printf("\tIsOnBoundary : %d\n", edge.IsOnBoundary());
		printf("\t[Associated HalfEdge %d]\n", edge.GetHalfEdge()->GetID().Data());
	}
	printf("\n");

	for (const auto& face : GetFaces())
	{
		printf("[Face %d]\n", face.GetID().Data());
		printf("\tIsBoundary : %d\n", face.IsBoundary());

		{
			auto h = face.GetHalfEdge();
			printf("\t");
			do
			{
				printf("[HalfEdge %d] -> ", h->GetID().Data());
				h = h->GetNext();
			} while (h != face.GetHalfEdge());
			printf("[HalfEdge %d]\n", face.GetHalfEdge()->GetID().Data());
		}

		{
			auto h = face.GetHalfEdge();
			printf("\t");
			do
			{
				printf("(v%d -> v%d) -> ", h->GetVertex()->GetID().Data(), h->GetEndVertex()->GetID().Data());
				h = h->GetNext();
			} while (h != face.GetHalfEdge());
			printf("(v%d -> v%d)\n", h->GetVertex()->GetID().Data(), h->GetEndVertex()->GetID().Data());
		}
	}
}

bool HalfEdgeMesh::IsTriangleMesh() const
{
	for (const auto& face : m_faces)
	{
		if (face.Degree() != 3U)
		{
			return false;
		}
	}

	return true;
}

uint32_t HalfEdgeMesh::Boundaries() const
{
	uint32_t count = 0U;
	for (const auto& face : m_faces)
	{
		if (face.IsBoundary())
		{
			++count;
		}
	}
	return count;
}

VertexRef HalfEdgeMesh::EmplaceVertex()
{
	VertexRef vertex;
	if (m_freeVertices.empty())
	{
		vertex = m_vertices.emplace(m_vertices.end(), Vertex(VertexID(m_nextVertexID++)));
	}
	else
	{
		vertex = m_freeVertices.begin();
		m_vertices.splice(m_vertices.end(), m_freeVertices, m_freeVertices.begin());
		*vertex = Vertex(VertexID(m_nextVertexID++));
	}

	vertex->SetHalfEdge(m_halfEdges.end());
	return vertex;
}

HalfEdgeRef HalfEdgeMesh::EmplaceHalfEdge()
{
	HalfEdgeRef halfEdge;
	if (m_freeHalfEdges.empty())
	{
		halfEdge = m_halfEdges.emplace(m_halfEdges.end(), HalfEdge(HalfEdgeID(m_nextHalfEdgeID++)));
	}
	else
	{
		halfEdge = m_freeHalfEdges.begin();
		m_halfEdges.splice(m_halfEdges.end(), m_freeHalfEdges, m_freeHalfEdges.begin());
		*halfEdge = HalfEdge(HalfEdgeID(m_nextHalfEdgeID++));
	}

	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetPrev(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());
	return halfEdge;
}

EdgeRef HalfEdgeMesh::EmplaceEdge()
{
	EdgeRef edge;
	if (m_freeEdges.empty())
	{
		edge = m_edges.emplace(m_edges.end(), Edge(EdgeID(m_nextEdgeID++)));
	}
	else
	{
		edge = m_freeEdges.begin();
		m_edges.splice(m_edges.end(), m_freeEdges, m_freeEdges.begin());
		*edge = Edge(EdgeID(m_nextEdgeID++));
	}

	edge->SetHalfEdge(m_halfEdges.end());
	return edge;
}

FaceRef HalfEdgeMesh::EmplaceFace(bool isBoundary)
{
	FaceRef face;
	if (m_freeFaces.empty())
	{
		face = m_faces.emplace(m_faces.end(), Face(FaceID(m_nextFaceID++), isBoundary));
	}
	else
	{
		face = m_freeFaces.begin();
		m_faces.splice(m_faces.end(), m_freeFaces, m_freeFaces.begin());
		*face = Face(FaceID(m_nextFaceID++), isBoundary);
	}

	face->SetHalfEdge(m_halfEdges.end());
	return face;
}

void HalfEdgeMesh::EraseVertex(VertexRef vertex)
{
	// clear data
	vertex->SetID(VertexID::Invalid());
	vertex->SetPosition(Point::Nan());

	// clear connectivity
	vertex->SetHalfEdge(m_halfEdges.end());

	m_freeVertices.splice(m_freeVertices.end(), m_vertices, vertex);
}

void HalfEdgeMesh::EraseHalfEdge(HalfEdgeRef halfEdge)
{
	// clear data
	halfEdge->SetID(HalfEdgeID::Invalid());

	// clear connectivity
	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetPrev(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());

	m_freeHalfEdges.splice(m_freeHalfEdges.end(), m_halfEdges, halfEdge);
}

void HalfEdgeMesh::EraseEdge(EdgeRef edge)
{
	// clear data
	edge->SetID(EdgeID::Invalid());

	// clear connectivity
	edge->SetHalfEdge(m_halfEdges.end());

	m_freeEdges.splice(m_freeEdges.end(), m_edges, edge);
}

void HalfEdgeMesh::EraseFace(FaceRef face)
{
	// clear data
	face->SetID(FaceID::Invalid());
	face->SetIsBoundary(false);

	// clear connectivity
	face->SetHalfEdge(m_halfEdges.end());

	m_freeFaces.splice(m_freeFaces.end(), m_faces, face);
}

VertexRef HalfEdgeMesh::AddVertex()
{
	return EmplaceVertex();
}

EdgeRef HalfEdgeMesh::AddEdge(VertexRef v0, VertexRef v1)
{
	assert(v0 != v1);

	auto edge = EmplaceEdge();
	auto v0v1 = EmplaceHalfEdge();
	auto v1v0 = EmplaceHalfEdge();

	edge->SetHalfEdge(v0v1);

	HalfEdge::SetNextAndPrev(v0v1, v1v0);
	v0v1->SetTwin(v1v0);
	v0v1->SetVertex(v0);
	v0v1->SetEdge(edge);

	HalfEdge::SetNextAndPrev(v1v0, v0v1);
	v1v0->SetTwin(v0v1);
	v1v0->SetVertex(v1);
	v1v0->SetEdge(edge);

	if (v0->GetHalfEdge() == m_halfEdges.end())
	{
		v0->SetHalfEdge(v0v1);
	}
	else
	{
		//
		//      in
		//     /
		//    /
		//   /
		// v0 ------- v1
		//   \
		//    \
		//     \
		//      out
		auto optFreeIncident = FindFreeIncident(v0->GetHalfEdge()->GetTwin(), v0->GetHalfEdge()->GetTwin());
		assert(optFreeIncident.has_value());
		if (optFreeIncident.has_value())
		{
			auto inV0 = optFreeIncident.value();
			auto outV0 = inV0->GetNext();
			HalfEdge::SetNextAndPrev(inV0, v0v1);
			HalfEdge::SetNextAndPrev(v1v0, outV0);
		}
	}

	if (v1->GetHalfEdge() == m_halfEdges.end())
	{
		v1->SetHalfEdge(v1v0);
	}
	else
	{
		//
		//       out
		//         \
		//          \
		//           \
		// v0 ------- v1
		//           /
		//          / 
		//         /
		//       in
		auto optFreeIncident = FindFreeIncident(v1->GetHalfEdge()->GetTwin(), v1->GetHalfEdge()->GetTwin());
		assert(optFreeIncident.has_value());
		if (optFreeIncident.has_value())
		{
			auto inV1 = optFreeIncident.value();
			auto outV1 = inV1->GetNext();
			HalfEdge::SetNextAndPrev(inV1, v1v0);
			HalfEdge::SetNextAndPrev(v0v1, outV1);
		}
	}

	return edge;
}

std::optional<FaceRef> HalfEdgeMesh::AddFace(const std::vector<HalfEdgeRef>& loop)
{
	assert(loop.size() > 2);

	HalfEdgeRef v0v1 = loop[0];

	// Check if it is OK to add half edge's loop.
	{
		HalfEdgeRef prev = m_halfEdges.end();
		for (auto h : loop)
		{
			if (h->GetFace() != m_faces.end())
			{
				return std::nullopt;
			}

			if (prev != m_halfEdges.end() && prev->GetEndVertex() != h->GetVertex())
			{
				return std::nullopt;
			}

			prev = h;
		}

		if (prev->GetEndVertex() != v0v1->GetVertex())
		{
			return std::nullopt;
		}
	}

	// Make sure that it will be a loop.
	{
		HalfEdgeRef prev = m_halfEdges.end();
		for (auto h : loop)
		{
			if (prev != m_halfEdges.end())
			{
				MakeAdjacent(prev, h);
			}
			prev = h;
		}
		MakeAdjacent(prev, v0v1);
	}

	// Add new face.
	auto face = EmplaceFace();
	face->SetHalfEdge(v0v1);

	for (auto h : loop)
	{
		h->SetFace(face);
	}

	return face;
}

void HalfEdgeMesh::RemoveVertex(VertexRef vertex)
{
	HalfEdgeRef h = vertex->GetHalfEdge();
	do
	{
		RemoveEdge(h->GetEdge());

		h = h->GetRotateNext();
	} while (h != vertex->GetHalfEdge());

	EraseVertex(vertex);
}

void HalfEdgeMesh::RemoveEdge(EdgeRef edge)
{
	HalfEdgeRef v0v1 = edge->GetHalfEdge();
	HalfEdgeRef v1v0 = v0v1->GetTwin();

	if (v0v1->GetFace() != m_faces.end())
	{
		RemoveFace(v0v1->GetFace());
	}

	if (v1v0->GetFace() != m_faces.end())
	{
		RemoveFace(v1v0->GetFace());
	}

	//
	//    in/out
	//    /  \
	//   /    \
	// v0______v1
	//  \      /
	//   \    /
	//   out/in
	//
	auto v0 = v0v1->GetVertex();
	auto v1 = v1v0->GetVertex();
	auto inV0 = v0v1->GetPrev();
	auto inV1 = v1v0->GetPrev();
	auto outV0 = v1v0->GetNext();
	auto outV1 = v0v1->GetNext();

	if (v0->GetHalfEdge() == v0v1)
	{
		v0->SetHalfEdge(outV0 != v0v1 ? outV0 : m_halfEdges.end());
	}
	inV0->SetNext(outV0);

	if (v1->GetHalfEdge() == v1v0)
	{
		v1->SetHalfEdge(outV1 != v1v0 ? outV1 : m_halfEdges.end());
	}
	HalfEdge::SetNextAndPrev(inV1, outV1);
	inV1->SetNext(outV1);

	EraseHalfEdge(v0v1);
	EraseHalfEdge(v1v0);
	EraseEdge(edge);
}

void HalfEdgeMesh::RemoveFace(FaceRef face)
{
	HalfEdgeRef h = face->GetHalfEdge();
	do
	{
		h->SetFace(m_faces.end());
		h = h->GetNext();
	} while (h != face->GetHalfEdge());

	EraseFace(face);
}

std::optional<HalfEdgeRef> HalfEdgeMesh::FindFreeIncident(HalfEdgeRef begin, HalfEdgeRef end)
{
	// Find an incident halfedge v1v0, v2v0, v3v0, ... which is free.
	// Free means the halfedge is standalone which doesn't reference a face.
	// 
	//    v1       v2
	//     \      /
	//		\    /
	//		 \  /
	//        v0 ------- v3
	assert(begin->GetEndVertex() == end->GetEndVertex());

	HalfEdgeRef h = begin;
	do
	{
		if (h->GetFace() == m_faces.end())
		{
			return h;
		}

		h = h->GetNext()->GetTwin();
	} while (h != end);

	return std::nullopt;
}

bool HalfEdgeMesh::MakeAdjacent(HalfEdgeRef in, HalfEdgeRef out)
{
	// Make inv0 adjacent with v0out.
	// v1v1 is not free half edge while fiv0 is free half edge.
	// 
	// Before:
	// a:
	//    in
	//     \      
	//		\    
	//		 \  
	//		  v0 --------- fi(free incident)
	//			\
	//			 \
	//			  \
	//             \
	//              out
	//
	// After:
	// a:
	//   fi        
	//     \      
	//		\    
	//		 \  
	//		  v0 --------- in
	//			\
	//			 \
	//			  \
	//             \
	//              out
	// 
	// Before:
	// b:
	//    in       v1
	//     \      /
	//		\    /
	//		 \  /
	//		  v0 --------- fi(free incident)
	//			\
	//			 \
	//			  \
	//             \
	//              out
	//
	// After:
	// b:
	//   fi        v1
	//     \      /
	//		\    /
	//		 \  /
	//		  v0 --------- in
	//			\
	//			 \
	//			  \
	//             \
	//              out
	assert(in->GetEndVertex() == out->GetVertex());

	if (in->GetNext() == out)
	{
		// Already adjacent.
		return true;
	}

	auto optFreeIncident = FindFreeIncident(out->GetTwin(), in);
	if (!optFreeIncident.has_value())
	{
		// From outv0 to inv0 in CCW. Can't find a free incident halfedge to swap.
		return false;
	}

	auto freeIncident = optFreeIncident.value();
	auto freeIncidentNext = freeIncident->GetNext();
	auto inNext = in->GetNext();
	auto outPre = out->GetPrev();

	HalfEdge::SetNextAndPrev(in, out);
	HalfEdge::SetNextAndPrev(freeIncident, inNext);
	// OutPre will be v1v0 because these two halfedges doesn't construct a loop.
	HalfEdge::SetNextAndPrev(outPre, freeIncidentNext);
	return true;
}

std::optional<EdgeRef> HalfEdgeMesh::FlipEdge(EdgeRef edge)
{
	// Rotate non-boundary edge in CCW : v0v1 -> v2v3, v1v0 -> v3v2
	// Before:
	// v3 ------- v1
	// |	    / |
	// |	  /	  |
	// |   /	  |
	// | /		  |
	// v0 ------- v2

	// After:
	// v3 ------- v1
	// | \	      |
	// |   \	  |
	// |     \ 	  |
	// |  		\ |
	// v0 ------- v2
	if (edge->IsOnBoundary())
	{
		return std::nullopt;
	}
	
	// Prepare data.
	auto v0v1 = edge->GetHalfEdge();
	auto v1v0 = v0v1->GetTwin();
	auto v0v2 = v1v0->GetNext();
	auto v1v3 = v0v1->GetNext();
	auto v0v1Prev = v0v1->GetPrev();
	auto v1v0Prev = v1v0->GetPrev();
	auto v0v2Next = v0v2->GetNext();
	auto v1v3Next = v1v3->GetNext();

	auto v0v1Face = v0v1->GetFace();
	auto v1v0Face = v1v0->GetFace();

	auto v0 = v0v1->GetVertex();
	auto v1 = v1v0->GetVertex();
	auto v2 = v0v2->GetEndVertex();
	auto v3 = v1v3->GetEndVertex();

	// Reassign connectivity.
	// If the vertex's half edge is on the fliping edge, we should update half edge's next half edge to it : v0v1 -> v0v2 -> ...
	if (v0->GetHalfEdge() == v0v1)
	{
		v0->SetHalfEdge(v0v2);
	}

	if (v1->GetHalfEdge() == v1v0)
	{
		v1->SetHalfEdge(v1v3);
	}

	// Rename variables in advance. Rotate CCW to get expected connectivity as below.
	auto v2v3 = v0v1;
	auto v3v2 = v1v0;

	// Change half edge's leaving vertex.
	v2v3->SetVertex(v2);
	v3v2->SetVertex(v3);

	// v2v3 and v3v2 is new added half edge so need to update next half edge.
	HalfEdge::SetNextAndPrev(v2v3, v1v3Next);
	HalfEdge::SetNextAndPrev(v3v2, v0v2Next);

	// The loop may contain 3 ~ n half edges. We only need to maintain the begin half edge and the end half edge.
	// For example, v0v1 will disconnect and v0v2 will connect. As a new added half edge, v0v2 should update its next half edge.
	HalfEdge::SetNextAndPrev(v0v1Prev, v0v2);
	HalfEdge::SetNextAndPrev(v0v2, v2v3);
	v0v2->SetFace(v0v1Face);

	HalfEdge::SetNextAndPrev(v1v0Prev, v1v3);
	HalfEdge::SetNextAndPrev(v1v3, v3v2);
	v1v3->SetFace(v1v0Face);

	// v0v2 and v1v3 changes its FaceRef. If face's half edge is v0v2 or v1v3, it needs to update.
	// The most convenient way is just to use twin half edge's face in opposite direction.
	if (v0v1Face->GetHalfEdge() == v1v3)
	{
		v0v1Face->SetHalfEdge(v0v2);
	}

	if (v1v0Face->GetHalfEdge() == v0v2)
	{
		v1v0Face->SetHalfEdge(v1v3);
	}

	// The edge connectivity data don't change.
	return edge;
}

std::optional<VertexRef> HalfEdgeMesh::SplitEdge(EdgeRef edge, float t)
{
	assert(t >= 0.0f && t <= 1.0f);

	// Prepare data.
	auto v0v1 = edge->GetHalfEdge();
	auto v1v0 = v0v1->GetTwin();

	bool v0v1OnBoundary = v0v1->GetFace() == GetFaces().end();
	bool v1v0OnBoundary = v1v0->GetFace() == GetFaces().end();
	if (v0v1OnBoundary &&
		v1v0OnBoundary)
	{
		// Two sides half edges are on boundary.
		return std::nullopt;
	}

	if (v0v1OnBoundary ||
		v1v0OnBoundary)
	{
		if (v0v1OnBoundary)
		{
			// Only process non-boundary side.
			std::swap(v0v1, v1v0);
		}

		auto v0v1Face = v0v1->GetFace();
		if (v0v1Face->Degree() != 3U)
		{
			return std::nullopt;
		}

		// One half edge is on boundary, one not.
		// Before:
		//      v1
		//     / |
		//	  /	 |
		//	v2	 |
		//	 \	 |
		//	  \  |
		//	   \ |
		//		v0
		//
		// After:
		//      v1
		//     / |
		//	  /	 |
		//	v2 - v3
		//	 \	 |
		//	  \  |
		//	   \ |
		//		v0

		// Prepare data.
		auto v1v2 = v0v1->GetNext();
		auto v2v0 = v1v2->GetNext();
		auto v1v0Next = v1v0->GetNext();

		auto v0 = v0v1->GetVertex();
		auto v1 = v1v0->GetVertex();
		auto v2 = v2v0->GetVertex();

		// Reuse old data as new data.
		auto v0v3 = v0v1;
		auto v1v3 = v1v0;
		auto v0v3Edge = edge;
		auto v0v3Face = v0v1Face;

		// Add new data.
		auto v3 = EmplaceVertex();
		auto v2v3 = EmplaceHalfEdge();
		auto v3v0 = EmplaceHalfEdge();
		auto v3v1 = EmplaceHalfEdge();
		auto v3v2 = EmplaceHalfEdge();
		auto v1v3Edge = EmplaceEdge();
		auto v2v3Edge = EmplaceEdge();
		auto v3v1Face = EmplaceFace();

		// Set vertex connectivity data.
		v3->SetHalfEdge(v3v1);
		v3->SetPosition(v0->GetPosition() * t + v1->GetPosition() * (1 - t));

		// Update connectivity to old data.
		HalfEdge::SetNextAndPrev(v1v2, v2v3);
		v1v2->SetFace(v3v1Face);

		// v1v0 is on boundary so don't set face for splited v1v3 and v3v0.
		HalfEdge::SetData(v1v3, v3v1, v1v2, v1, v1v3Edge, m_faces.end());
		HalfEdge::SetData(v3v0, v0v3, v1v0Next, v3, v0v3Edge, m_faces.end());

		// Set connectivity data for other boundary half edges.
		HalfEdge::SetData(v0v3, v3v0, v3v2, v0, v0v3Edge, v0v3Face);
		HalfEdge::SetData(v3v1, v1v3, v1v2, v3, v1v3Edge, v3v1Face);
		HalfEdge::SetData(v3v2, v2v3, v2v0, v3, v2v3Edge, v0v3Face);
		HalfEdge::SetData(v2v3, v3v2, v3v1, v2, v2v3Edge, v3v1Face);

		// Set new edges.
		v0v3Edge->SetHalfEdge(v0v3);
		v1v3Edge->SetHalfEdge(v1v3);
		v2v3Edge->SetHalfEdge(v2v3);

		// Set faces.
		if (v0v3Face->GetHalfEdge() == v0v1)
		{
			v0v3Face->SetHalfEdge(v0v3);
		}
		v3v1Face->SetHalfEdge(v3v1);

		return v3;
	}

	auto v0v1Face = v0v1->GetFace();
	auto v1v0Face = v1v0->GetFace();
	if (v0v1Face->Degree() != 3 ||
		v1v0Face->Degree() != 3)
	{
		return std::nullopt;
	}

	// Both two half edges are not on boundary.
	// Before:
	//       v1
	//     / || \
	//	  /	 ||  \
	//	v2	 ||   v3
	//	 \	 ||   /
	//	  \  ||  /
	//	   \ || /
	//		 v0
	//
	// After:
	//       v1
	//     / || \
	//	  /	 ||  \
	//	v2 - v4 - v3
	//	 \	 ||   /
	//	  \  ||  /
	//	   \ || /
	//		 v0
	
	// Prepare data.
	auto v1v2 = v0v1->GetNext();
	auto v2v0 = v1v2->GetNext();
	auto v0v3 = v1v0->GetNext();
	auto v3v1 = v0v3->GetNext();

	auto v0 = v0v1->GetVertex();
	auto v1 = v1v0->GetVertex();
	auto v2 = v2v0->GetVertex();
	auto v3 = v3v1->GetVertex();

	// Reuse old data as new data.
	auto v0v4 = v0v1;
	auto v1v4 = v1v0;
	auto v0v4Edge = edge;
	auto v0v4Face = v0v1->GetFace();
	auto v1v4Face = v1v0->GetFace();

	// Add new data.
	auto v4 = EmplaceVertex();
	auto v2v4 = EmplaceHalfEdge();
	auto v3v4 = EmplaceHalfEdge();
	auto v4v0 = EmplaceHalfEdge();
	auto v4v1 = EmplaceHalfEdge();
	auto v4v2 = EmplaceHalfEdge();
	auto v4v3 = EmplaceHalfEdge();
	auto v1v4Edge = EmplaceEdge();
	auto v2v4Edge = EmplaceEdge();
	auto v3v4Edge = EmplaceEdge();
	auto v4v1Face = EmplaceFace();
	auto v4v0Face = EmplaceFace();
	
	// Set vertex connectivity data.
	v4->SetHalfEdge(v4v1);
	v4->SetPosition(v0->GetPosition() * t + v1->GetPosition() * (1 - t));

	// Update connectivity to old data.
	HalfEdge::SetNextAndPrev(v1v2, v2v4);
	v1v2->SetFace(v4v1Face);
	HalfEdge::SetNextAndPrev(v0v3, v3v4);
	v0v3->SetFace(v4v0Face);

	// Set old half edges.
	HalfEdge::SetData(v0v4, v4v0, v4v2, v0, v0v4Edge, v0v4Face);
	HalfEdge::SetData(v1v4, v4v1, v4v3, v1, v1v4Edge, v1v4Face);

	// Set new half edges.
	HalfEdge::SetData(v2v4, v4v2, v4v1, v2, v2v4Edge, v4v1Face);
	HalfEdge::SetData(v3v4, v4v3, v4v0, v3, v3v4Edge, v4v0Face);
	HalfEdge::SetData(v4v0, v0v4, v0v3, v4, v0v4Edge, v4v0Face);
	HalfEdge::SetData(v4v1, v1v4, v1v2, v4, v1v4Edge, v4v1Face);
	HalfEdge::SetData(v4v2, v2v4, v2v0, v4, v2v4Edge, v0v4Face);
	HalfEdge::SetData(v4v3, v3v4, v3v1, v4, v3v4Edge, v1v4Face);

	// Set new edges.
	v0v4Edge->SetHalfEdge(v0v4);
	v1v4Edge->SetHalfEdge(v1v4);
	v2v4Edge->SetHalfEdge(v2v4);
	v3v4Edge->SetHalfEdge(v3v4);

	// Set faces.
	if (v0v4Face->GetHalfEdge() == v1v2)
	{
		v0v4Face->SetHalfEdge(v0v4);
	}

	if (v1v4Face->GetHalfEdge() == v0v3)
	{
		v1v4Face->SetHalfEdge(v1v4);
	}

	v4v1Face->SetHalfEdge(v4v1);
	v4v0Face->SetHalfEdge(v4v0);

	return v4;
}

std::optional<VertexRef> HalfEdgeMesh::CollapseEdge(EdgeRef edge, float t)
{
	assert(t >= 0.0f && t <= 1.0f);

	// Collapse Edge between v0 and v1.
	// Before:
	// a:
	//		 ____________
	//      /\    /\    /\
	//     /  \  /  \  /  \
	//    /____\/____\/____\
	//    \    /\    /\    /
	//     \  /  \  /  \  /
	//      \/____\/____\/
	//
	// b:
	//		 ____________
	//      /\    /\    /\
	//     /  \  /  \  /  \
	//    /____\/____\/____\
	//    \    /\     \    /
	//     \  /  \     \  /
	//      \/____\_____\/
	// 
	// c:
	//		 ____________
	//      /\    /     /\
	//     /  \  /     /  \
	//    /____\/____ /____\
	//    \    /\     \    /
	//     \  /  \     \  /
	//      \/____\_____\/
	// 
	// After:
	//		 ______
	//      /\    /\
	//     /  \  /  \
	//    /____\/____\
	//    \    /\    /
	//     \  /  \  /
	//      \/____\/
	//
	auto v0v1 = edge->GetHalfEdge();
	auto v1v0 = v0v1->GetTwin();
	auto v0 = v0v1->GetVertex();
	auto v1 = v1v0->GetVertex();
	
	{
		// Validate if mesh is OK to collapse.
		uint32_t count = 2U;
		if (v0v1->GetFace()->Degree() > 3)
		{
			--count;
		}

		if (v1v0->GetFace()->Degree() > 3)
		{
			--count;
		}

		HalfEdgeRef h0 = v0v1;
		do
		{
			VertexRef v0v1End = h0->GetEndVertex();
			HalfEdgeRef h1 = v1v0;
			do
			{
				VertexRef v1v0End = h1->GetEndVertex();
				if (v0v1End == v1v0End)
				{
					if (0U == count)
					{
						return std::nullopt;
					}

					--count;
				}

				h1 = h1->GetRotateNext();
			} while (h1 != v1v0);

			h0 = h0->GetRotateNext();
		} while (h0 != v0v1);
	}

	if (v0->IsOnBoundary() && v1->IsOnBoundary() && !edge->IsOnBoundary())
	{
		//		 _____v0_____
		//      /\    /\    /\
		//     /  \  /  \  /  \
		//    /____\/____\/____\
		//    \    /\    /\    /
		//     \  /  \  /  \  /
		//      \/____v1____\/
		return std::nullopt;
	}

	if (1U == v0->Degree())
	{
		//	   v0      ______
		//       \    /\    /\
		//        \  /  \  /  \
		//     ____v1____\/____\
		//    \    /\    /\    /
		//     \  /  \  /  \  /
		//      \/____\/____\/
		RemoveVertex(v0);
		return v1;
	}

	if (1U == v1->Degree())
	{
		// Ditto.
		RemoveVertex(v1);
		return v0;
	}

	// Or you can reuse v0 or v1. Only Remove one vertex.
	auto v = AddVertex();
	if (v0v1->GetPrev()->GetTwin()->GetFace() != v1v0->GetFace())
	{
		v->SetHalfEdge(v0v1->GetPrev()->GetTwin());
	}
	else
	{
		v->SetHalfEdge(v1v0->GetPrev()->GetTwin());
	}

	{
		HalfEdgeRef h = v0v1;
		do
		{
			h->SetVertex(v);
			h = h->GetRotateNext();
		} while (h != v0v1);
	}

	{
		HalfEdgeRef h = v1v0;
		do
		{
			h->SetVertex(v);
			h = h->GetRotateNext();
		} while (h != v1v0);
	}

	auto ProcessHalfEdge = [](HalfEdgeMesh* pHem, HalfEdgeRef v0v1)
	{
		auto v0v1Face = v0v1->GetFace();
		if (3U == v0v1Face->Degree())
		{
			// When face degree is 3, it is time to delete non-boundary face.
			auto v0v1Next = v0v1->GetNext();
			auto v0v1Prev = v0v1->GetPrev();
			auto v0v1NextTwin = v0v1Next->GetTwin();
			auto v0v1PrevTwin = v0v1Prev->GetTwin();
			
			auto e = pHem->EmplaceEdge();
			v0v1NextTwin->SetTwin(v0v1PrevTwin);
			v0v1NextTwin->SetEdge(e);
			v0v1PrevTwin->SetTwin(v0v1NextTwin);
			v0v1PrevTwin->SetEdge(e);
			e->SetHalfEdge(v0v1NextTwin);
			
			pHem->EraseEdge(v0v1Next->GetEdge());
			pHem->EraseEdge(v0v1Prev->GetEdge());
			pHem->EraseHalfEdge(v0v1Next);
			pHem->EraseHalfEdge(v0v1Prev);
			if (!v0v1Face->IsBoundary())
			{
				pHem->EraseFace(v0v1Face);
			}
		}
		else
		{
			if (!v0v1Face->IsBoundary() && v0v1Face->GetHalfEdge() == v0v1)
			{
				v0v1Face->SetHalfEdge(v0v1->GetNext());
			}
			v0v1->GetPrev()->SetNext(v0v1->GetNext());
		}
	};

	ProcessHalfEdge(this, v0v1);
	ProcessHalfEdge(this, v1v0);

	EraseVertex(v0);
	EraseVertex(v1);
	EraseHalfEdge(v0v1);
	EraseHalfEdge(v1v0);
	EraseEdge(edge);

	return v;
}

}