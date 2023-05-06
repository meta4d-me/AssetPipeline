#pragma once

#include "Framework/IProducer.h"
#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Math/MeshGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"

#include <cfloat>
#include <string>

namespace cdtools
{

class ProgressiveMeshProducer final : public IProducer
{
public:
	ProgressiveMeshProducer() = delete;
	explicit ProgressiveMeshProducer(const char* pFilePath) : m_filePath(pFilePath) {}
	ProgressiveMeshProducer(const ProgressiveMeshProducer&) = delete;
	ProgressiveMeshProducer& operator=(const ProgressiveMeshProducer&) = delete;
	ProgressiveMeshProducer(ProgressiveMeshProducer&&) = delete;
	ProgressiveMeshProducer& operator=(ProgressiveMeshProducer&&) = delete;
	virtual ~ProgressiveMeshProducer() {}

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override
	{
		pSceneDatabase->SetName("ProgressiveMesh");

		for (cd::Mesh& mesh : pSceneDatabase->GetMeshes())
		{
			uint32_t vertexCount = mesh.GetVertexCount();
			uint32_t polygonCount = mesh.GetPolygonCount();

			std::vector<float> vertexEdgeCollapseCosts;
			vertexEdgeCollapseCosts.resize(vertexCount);
			std::fill(vertexEdgeCollapseCosts.begin(), vertexEdgeCollapseCosts.end(), FLT_MAX);

			std::vector<cd::VertexID> vertexEdgeCollapseTargets;
			vertexEdgeCollapseTargets.resize(vertexCount);
			std::fill(vertexEdgeCollapseTargets.begin(), vertexEdgeCollapseTargets.end(), cd::VertexID::InvalidID);

			// Calculate face normals.
			std::vector<cd::Direction> polygonNormals;
			polygonNormals.resize(polygonCount);
			for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
			{
				const cd::Polygon& polygon = mesh.GetPolygon(polygonIndex);
				cd::Point v0 = mesh.GetVertexPosition(polygon[0].Data());
				cd::Point v1v0 = mesh.GetVertexPosition(polygon[1].Data()) - v0;
				cd::Point v2v0 = mesh.GetVertexPosition(polygon[2].Data()) - v0;
				polygonNormals[polygonIndex] = v1v0.Cross(v2v0).Normalize();
			}

			// Calculate edge collapse data.
			for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
			{
				if (0U == mesh.GetVertexAdjacentVertexCount(vertexIndex))
				{
					// Nothing to collapse.
					continue;
				}

				// Loop every edge to calculate the min collapse cost.
				for (cd::VertexID adjVertexID : mesh.GetVertexAdjacentVertexArray(vertexIndex))
				{
					float collapseCost = CalculateEdgeCollapseCost(mesh, vertexIndex, adjVertexID.Data(), polygonNormals);
					if (collapseCost < vertexEdgeCollapseCosts[vertexIndex])
					{
						vertexEdgeCollapseCosts[vertexIndex] = collapseCost;
						vertexEdgeCollapseTargets[vertexIndex] = adjVertexID;
					}
				}
			}

			// Start to collapse edge.
			auto GetMinCollapseCostVertexIndex = [&vertexEdgeCollapseCosts, &mesh]()
			{
				float minCost = FLT_MAX;
				uint32_t minCostVertexIndex = cd::VertexID::InvalidID;
				for (uint32_t vertexIndex = 0U, vertexCount = mesh.GetVertexCount(); vertexIndex < vertexCount; ++vertexIndex)
				{
					if (vertexEdgeCollapseCosts[vertexIndex] < minCost)
					{
						minCost = vertexEdgeCollapseCosts[vertexIndex];
						minCostVertexIndex = vertexIndex;
					}
				}

				return minCostVertexIndex;
			};

			for (int collapseIndex = vertexCount - 1; collapseIndex >= 0; --collapseIndex)
			{
				uint32_t v0 = GetMinCollapseCostVertexIndex();
				uint32_t v1 = vertexEdgeCollapseTargets[v0].Data();

				printf("Collapse Edge (%u, %u)\n", v0, v1);
				CollapseEdge(mesh, v0, v1, polygonNormals);
			}
		}
	}

private:
	void CollapseEdge(cd::Mesh& mesh, uint32_t v0, uint32_t v1, const std::vector<cd::Direction>& polygonNormals)
	{
		assert(v0 != cd::VertexID::InvalidID);
		assert(v1 != cd::VertexID::InvalidID);

		// Polygons prepared to remove.
		std::vector<cd::PolygonID> removePolygonIDs;

		uint32_t adjPolygonCount = mesh.GetVertexAdjacentPolygonCount(v0);
		const cd::PolygonIDArray& adjPolygons = mesh.GetVertexAdjacentPolygonArray(v0);
		for (uint32_t polygonIndex = 0U; polygonIndex < adjPolygonCount; ++polygonIndex)
		{
			cd::PolygonID adjacentPolygonID = adjPolygons[polygonIndex];
			const cd::Polygon& adjacentPolygon = mesh.GetPolygon(adjacentPolygonID.Data());
			if (adjacentPolygon.Contains(cd::VertexID(v1)))
			{
				// Delete Edge (v0, v1) adjacent polygons
				removePolygonIDs.push_back(adjacentPolygonID);
			}
		}

		std::sort(removePolygonIDs.begin(), removePolygonIDs.end(), [](cd::PolygonID lhs, cd::PolygonID rhs) { return lhs > rhs; });
		for (cd::PolygonID removePolygonID : removePolygonIDs)
		{
			// 1. Delete polygon in the mesh polygon array
			// 2. Loop through polygon's every vertex which maintains adjacent polygon id. Need to update.
			// 
		}

		// Some unused vertices may appear. Check to remove them.
	}

	float CalculateEdgeCollapseCost(const cd::Mesh& mesh, uint32_t v0, uint32_t v1, const std::vector<cd::Direction>& polygonNormals)
	{
		cd::VertexID v1ID(v1);
		cd::PolygonIDArray sharedPolygons;
		for (cd::PolygonID polygonID : mesh.GetVertexAdjacentPolygonArray(v0))
		{
			const cd::Polygon& polygon = mesh.GetPolygon(polygonID.Data());
			if (polygon.Contains(v1ID))
			{
				sharedPolygons.push_back(polygonID);
			}
		}

		float curvature = 0.0f;
		for (cd::PolygonID polygonID : mesh.GetVertexAdjacentPolygonArray(v0))
		{
			float minCurvature = 1.0f;
			const cd::Direction& v0FaceNormal = polygonNormals[polygonID.Data()];
			for (cd::PolygonID sharedPolygonID : sharedPolygons)
			{
				const cd::Direction& sharedFaceNormal = polygonNormals[sharedPolygonID.Data()];
				float faceNormalDot = v0FaceNormal.Dot(sharedFaceNormal);
				float t = (1 - faceNormalDot) * 0.5f;
				if (t < minCurvature)
				{
					minCurvature = t;
				}
			}
			
			if (minCurvature > curvature)
			{
				curvature = minCurvature;
			}
		}

		float edgeLength = (mesh.GetVertexPosition(v0) - mesh.GetVertexPosition(v1)).Length();
		return edgeLength * curvature;
	}

private:
	std::string m_filePath;
};

}