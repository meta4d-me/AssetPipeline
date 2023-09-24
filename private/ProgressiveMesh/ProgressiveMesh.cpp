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

std::pair<std::vector<uint32_t>, std::vector<uint32_t>> ProgressiveMesh::BuildCollapseOperations()
{
	return m_pProgressiveMeshImpl->BuildCollapseOperations();
}

}