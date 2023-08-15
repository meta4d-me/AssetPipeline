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

//helper used to get set of pointers in a list (used by validate and describe):
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
				halfEdge->SetPrev(prev);
				prev->SetNext(halfEdge);
			}
			prev = halfEdge;
		}

		// Connect last half edge to first half edge to be a loop.
		face->GetHalfEdge()->SetPrev(prev);
		prev->SetNext(face->GetHalfEdge());
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
	//uint32_t vertexIndex = 0U;
	//for (const auto& vertex : halfEdgeMesh.GetVertices())
	//{
	//	assert(vertexIndex < mesh.GetVertexCount());
	//	HalfEdgeRef h = vertex.GetHalfEdge();
	//
	//	do
	//	{
	//		if (!h->GetFace()->IsBoundary())
	//		{
	//			// Only copy base uv.
	//			h->SetCornerUV(mesh.GetVertexUV(0U, vertexIndex));
	//			h->SetCornerNormal(mesh.GetVertexNormal(vertexIndex));
	//		}
	//
	//		h = h->GetTwin()->GetNext();
	//	} while (h != vertex.GetHalfEdge());
	//
	//	++vertexIndex;
	//}
	//assert(vertexIndex == mesh.GetVertexCount());

	return halfEdgeMesh;
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

EdgeRef HalfEdgeMesh::EmplaceEdge()
{
	EdgeRef edge;
	if (m_freeEdges.empty())
	{
		edge = m_edges.emplace(m_edges.end(), Edge(EdgeID(m_nextEdgeID++)));
	}
	else
	{
		edge = m_edges.begin();
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
		face = m_faces.begin();
		m_faces.splice(m_faces.end(), m_freeFaces, m_freeFaces.begin());
		*face = Face(FaceID(m_nextFaceID++), isBoundary);
	}

	face->SetHalfEdge(m_halfEdges.end());
	return face;
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
		halfEdge = m_halfEdges.begin();
		m_halfEdges.splice(m_halfEdges.end(), m_freeHalfEdges, m_freeHalfEdges.begin());
		*halfEdge = HalfEdge(HalfEdgeID(m_nextHalfEdgeID++));
	}

	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetPrev(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());
	return halfEdge;
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

void HalfEdgeMesh::EraseHalfEdge(HalfEdgeRef halfEdge)
{
	// clear data
	halfEdge->SetID(HalfEdgeID::Invalid());

	// clear connectivity
	halfEdge->SetTwin(m_halfEdges.end());
	halfEdge->SetPrev(m_halfEdges.end());
	halfEdge->SetNext(m_halfEdges.end());
	halfEdge->SetVertex(m_vertices.end());
	halfEdge->SetEdge(m_edges.end());
	halfEdge->SetFace(m_faces.end());

	m_freeHalfEdges.splice(m_freeHalfEdges.end(), m_halfEdges, halfEdge);
}

bool HalfEdgeMesh::Validate() const
{
	for (const auto& vertex : GetVertices())
	{
		if (!vertex.Validate())
		{
			return false;
		}
	}

	for (const auto& edge : GetEdges())
	{
		if (!edge.Validate())
		{
			return false;
		}
	}

	for (const auto& face : GetFaces())
	{
		if (!face.Validate())
		{
			return false;
		}
	}

	for (const auto& halfEdge : GetHalfEdges())
	{
		if (!halfEdge.Validate())
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

			h = h->GetTwin()->GetNext();
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

std::optional<VertexRef> HalfEdgeMesh::CollapseEdge(EdgeRef edge, float t)
{
	HalfEdgeRef v0v1 = edge->GetHalfEdge();
	HalfEdgeRef v1v0 = v0v1->GetTwin();

	VertexRef v0 = v0v1->GetVertex();
	VertexRef v1 = v1v0->GetVertex();
	
	// Keep v0 and move it to a new position between v0 and v1.  
	v0->SetPosition(v0->GetPosition() * t + v1->GetPosition() * (1 - t));
	v0->SetHalfEdge(v0v1->GetNext());

	return std::nullopt;
}

}