#pragma once

#include "Math/Box.hpp"
#include "Scene/Types.h"

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
	static ProgressiveMesh FromIndexedFaces(const std::vector<cd::Point>& vertices, const std::vector<cd::PolygonGroup>& polygonGroups);
	static ProgressiveMesh FromIndexedMesh(const cd::Mesh& mesh);

public:
	ProgressiveMesh() = default;
	ProgressiveMesh(const ProgressiveMesh&) = delete;
	ProgressiveMesh& operator=(const ProgressiveMesh&) = delete;
	ProgressiveMesh(ProgressiveMesh&&);
	ProgressiveMesh& operator=(ProgressiveMesh&&);
	~ProgressiveMesh();
	
	void InitBoundary(const cd::AABB& aabb);
	void InitBoundary(const cd::Mesh& mesh);
	void InitBoundary(const std::vector<cd::Point>& vertices, const std::vector<cd::PolygonGroup>& polygonGroups);
	std::pair<std::vector<uint32_t>, std::vector<uint32_t>> BuildCollapseOperations();
	cd::Mesh GenerateLodMesh(float percent, const cd::Mesh* pSourceMesh = nullptr);
	cd::Mesh GenerateLodMesh(float percent, uint32_t minFaceCount, const cd::Mesh* pSourceMesh = nullptr);
	cd::Mesh GenerateLodMesh(uint32_t targetFaceCount, const cd::Mesh* pSourceMesh = nullptr);

private:
	pm::ProgressiveMeshImpl* m_pProgressiveMeshImpl;
};

}