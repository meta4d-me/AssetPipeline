#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "HalfEdgeMeshImpl.h"

#include "Scene/Mesh.h"

namespace cd
{

HalfEdgeMesh::HalfEdgeMesh() = default;
HalfEdgeMesh::HalfEdgeMesh(HalfEdgeMesh&&) = default;
HalfEdgeMesh& HalfEdgeMesh::operator=(HalfEdgeMesh&&) = default;
HalfEdgeMesh::~HalfEdgeMesh() = default;

HalfEdgeMesh HalfEdgeMesh::FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons)
{
	HalfEdgeMesh halfEdgeMesh;
	halfEdgeMesh.m_pHalfEdgeMeshImpl = new hem::HalfEdgeMeshImpl();
	halfEdgeMesh.m_pHalfEdgeMeshImpl->FromIndexedFaces(vertices, polygons);
	return halfEdgeMesh;
}

HalfEdgeMesh HalfEdgeMesh::FromIndexedMesh(const cd::Mesh& mesh)
{
	return FromIndexedFaces(mesh.GetVertexPositions(), mesh.GetPolygons());
}

std::list<hem::Vertex>& HalfEdgeMesh::GetVertices()
{
	return m_pHalfEdgeMeshImpl->GetVertices();
}

const std::list<hem::Vertex>& HalfEdgeMesh::GetVertices() const
{
	return m_pHalfEdgeMeshImpl->GetVertices();
}

std::list<hem::Edge>& HalfEdgeMesh::GetEdges()
{
	return m_pHalfEdgeMeshImpl->GetEdges();
}

const std::list<hem::Edge>& HalfEdgeMesh::GetEdges() const
{
	return m_pHalfEdgeMeshImpl->GetEdges();
}

std::list<hem::Face>& HalfEdgeMesh::GetFaces()
{
	return m_pHalfEdgeMeshImpl->GetFaces();
}

const std::list<hem::Face>& HalfEdgeMesh::GetFaces() const
{
	return m_pHalfEdgeMeshImpl->GetFaces();
}

std::list<hem::HalfEdge>& HalfEdgeMesh::GetHalfEdges()
{
	return m_pHalfEdgeMeshImpl->GetHalfEdges();
}

const std::list<hem::HalfEdge>& HalfEdgeMesh::GetHalfEdges() const
{
	return m_pHalfEdgeMeshImpl->GetHalfEdges();
}

bool HalfEdgeMesh::IsValid() const
{
	return m_pHalfEdgeMeshImpl->IsValid();
}

void HalfEdgeMesh::Dump() const
{
	return m_pHalfEdgeMeshImpl->Dump();
}

bool HalfEdgeMesh::IsTriangleMesh() const
{
	return m_pHalfEdgeMeshImpl->IsTriangleMesh();
}

uint32_t HalfEdgeMesh::Boundaries() const
{
	return m_pHalfEdgeMeshImpl->Boundaries();
}

hem::VertexRef HalfEdgeMesh::AddVertex()
{
	return m_pHalfEdgeMeshImpl->AddVertex();
}

hem::EdgeRef HalfEdgeMesh::AddEdge(hem::VertexRef v0, hem::VertexRef v1)
{
	return m_pHalfEdgeMeshImpl->AddEdge(v0, v1);
}

std::optional<hem::FaceRef> HalfEdgeMesh::AddFace(const std::vector<hem::HalfEdgeRef>& loop)
{
	return m_pHalfEdgeMeshImpl->AddFace(loop);
}

void HalfEdgeMesh::RemoveVertex(hem::VertexRef vertex)
{
	return m_pHalfEdgeMeshImpl->RemoveVertex(vertex);
}

void HalfEdgeMesh::RemoveEdge(hem::EdgeRef edge)
{
	return m_pHalfEdgeMeshImpl->RemoveEdge(edge);
}

void HalfEdgeMesh::RemoveFace(hem::FaceRef face)
{
	return m_pHalfEdgeMeshImpl->RemoveFace(face);
}

std::optional<hem::EdgeRef> HalfEdgeMesh::FlipEdge(hem::EdgeRef edge)
{
	return m_pHalfEdgeMeshImpl->FlipEdge(edge);
}

std::optional<hem::VertexRef> HalfEdgeMesh::SplitEdge(hem::EdgeRef edge, float t)
{
	return m_pHalfEdgeMeshImpl->SplitEdge(edge, t);
}

std::optional<hem::VertexRef> HalfEdgeMesh::CollapseEdge(hem::EdgeRef edge, float t)
{
	return m_pHalfEdgeMeshImpl->CollapseEdge(edge, t);
}

}