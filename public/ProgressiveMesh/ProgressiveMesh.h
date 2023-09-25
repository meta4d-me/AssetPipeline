#pragma once

#include "Scene/ObjectID.h"

namespace cd
{

class Mesh;

namespace pm
{

class ProgressiveMeshImpl;

}

class CORE_API ProgressiveMesh
{
public:
	static ProgressiveMesh FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<std::vector<cd::VertexID>>& polygons);
	static ProgressiveMesh FromIndexedMesh(const cd::Mesh& mesh);

public:
	ProgressiveMesh() = default;
	ProgressiveMesh(const ProgressiveMesh&) = delete;
	ProgressiveMesh& operator=(const ProgressiveMesh&) = delete;
	ProgressiveMesh(ProgressiveMesh&&);
	ProgressiveMesh& operator=(ProgressiveMesh&&);
	~ProgressiveMesh();
	
	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> BuildCollapseOperations();

private:
	pm::ProgressiveMeshImpl* m_pProgressiveMeshImpl;
};

}