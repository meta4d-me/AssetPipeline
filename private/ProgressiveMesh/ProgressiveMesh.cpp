#include "ProgressiveMesh/ProgressiveMesh.h"
#include "ProgressiveMeshImpl.h"

#include "Scene/Mesh.h"

namespace cd
{

ProgressiveMesh ProgressiveMesh::FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons)
{
	ProgressiveMesh progressiveMesh;
	progressiveMesh.m_pProgressiveMeshImpl = new pm::ProgressiveMeshImpl();
	progressiveMesh.m_pProgressiveMeshImpl->FromIndexedFaces(vertices, polygons);
	return progressiveMesh;
}

ProgressiveMesh ProgressiveMesh::FromIndexedMesh(const cd::Mesh& mesh)
{
	return FromIndexedFaces(mesh.GetVertexPositions(), mesh.GetPolygons());
}

ProgressiveMesh::ProgressiveMesh(ProgressiveMesh&& rhs)
{
	*this = cd::MoveTemp(rhs);
}

ProgressiveMesh& ProgressiveMesh::operator=(ProgressiveMesh&& rhs)
{
	std::swap(m_pProgressiveMeshImpl, rhs.m_pProgressiveMeshImpl);
	return *this;
}

ProgressiveMesh::~ProgressiveMesh()
{
	if (m_pProgressiveMeshImpl)
	{
		delete m_pProgressiveMeshImpl;
		m_pProgressiveMeshImpl = nullptr;
	}
}

void ProgressiveMesh::InitBoundary(const cd::Mesh& mesh)
{
	return m_pProgressiveMeshImpl->InitBoundary(mesh.GetVertexPositions(), mesh.GetPolygons());
}

void ProgressiveMesh::InitBoundary(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons)
{
	return m_pProgressiveMeshImpl->InitBoundary(vertices, polygons);
}

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> ProgressiveMesh::BuildCollapseOperations()
{
	return m_pProgressiveMeshImpl->BuildCollapseOperations();
}

cd::Mesh ProgressiveMesh::GenerateLodMesh(float percent, const cd::Mesh* pSourceMesh)
{
	return m_pProgressiveMeshImpl->GenerateLodMesh(percent, pSourceMesh);
}

cd::Mesh ProgressiveMesh::GenerateLodMesh(uint32_t targetFaceCount, const cd::Mesh* pSourceMesh)
{
	return m_pProgressiveMeshImpl->GenerateLodMesh(targetFaceCount, pSourceMesh);
}

}