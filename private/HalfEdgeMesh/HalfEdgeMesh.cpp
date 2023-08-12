#include "HalfEdgeMesh/HalfEdgeMesh.h"

#include "HalfEdgeMesh/Edge.h"
#include "HalfEdgeMesh/Face.h"
#include "HalfEdgeMesh/HalfEdge.h"
#include "Scene/Mesh.h"
#include "HalfEdgeMesh/Vertex.h"

#include <map>
#include <unordered_map>

namespace cd::hem
{

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
				prev->SetNext(halfEdge);
			}
			prev = halfEdge;
		}

		// Connect last half edge to first half edge to be a loop.
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
	return HalfEdgeMesh::FromIndexedFaces(mesh.GetVertexPositions(), mesh.GetPolygons());
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

	return true;
}

}