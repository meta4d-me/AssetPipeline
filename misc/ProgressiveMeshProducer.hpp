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

//class ProgressiveMeshProducer final : public IProducer
//{
//public:
//	ProgressiveMeshProducer() = delete;
//	explicit ProgressiveMeshProducer(const char* pFilePath) : m_filePath(pFilePath) {}
//	ProgressiveMeshProducer(const ProgressiveMeshProducer&) = delete;
//	ProgressiveMeshProducer& operator=(const ProgressiveMeshProducer&) = delete;
//	ProgressiveMeshProducer(ProgressiveMeshProducer&&) = delete;
//	ProgressiveMeshProducer& operator=(ProgressiveMeshProducer&&) = delete;
//	virtual ~ProgressiveMeshProducer() {}
//
//	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override
//	{
//		pSceneDatabase->SetName("ProgressiveMesh");
//
//		for (cd::Mesh& mesh : pSceneDatabase->GetMeshes())
//		{
//			ProcessMesh(mesh);
//		}
//	}
//
//private:
//	void ProcessMesh(cd::Mesh& mesh)
//	{
//		// Query mesh basic information.
//		uint32_t originVertexCount = mesh.GetVertexCount();
//		uint32_t originPolygonCount = mesh.GetPolygonCount();
//
//		// Prepare for vertex data.
//		std::vector<float> vertexEdgeCollapseCosts;
//		std::vector<cd::VertexID> vertexEdgeCollapseTargets;
//		{
//			vertexEdgeCollapseCosts.resize(originVertexCount);
//			std::fill(vertexEdgeCollapseCosts.begin(), vertexEdgeCollapseCosts.end(), FLT_MAX);
//
//			vertexEdgeCollapseTargets.resize(originVertexCount);
//			std::fill(vertexEdgeCollapseTargets.begin(), vertexEdgeCollapseTargets.end(), cd::VertexID::InvalidID);
//		}
//
//		// Prepare for polygon data.
//		std::vector<cd::Direction> polygonNormals;
//		{
//			polygonNormals.resize(originPolygonCount);
//			for (uint32_t polygonIndex = 0U; polygonIndex < originPolygonCount; ++polygonIndex)
//			{
//				const cd::Polygon& polygon = mesh.GetPolygon(polygonIndex);
//				cd::Point v0 = mesh.GetVertexPosition(polygon[0].Data());
//				cd::Point v1v0 = mesh.GetVertexPosition(polygon[1].Data()) - v0;
//				cd::Point v2v0 = mesh.GetVertexPosition(polygon[2].Data()) - v0;
//				polygonNormals[polygonIndex] = v1v0.Cross(v2v0).Normalize();
//			}
//		}
//
//		// Calculate edge collapse data.
//		for (uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
//		{
//			if (0U == mesh.GetVertexAdjacentVertexCount(vertexIndex))
//			{
//				// Orphan vertex, nothing to collapse.
//				continue;
//			}
//
//			// Loop every edge to calculate the min collapse cost.
//			for (cd::VertexID adjVertexID : mesh.GetVertexAdjacentVertexArray(vertexIndex))
//			{
//				float collapseCost = CalculateEdgeCollapseCost(mesh, vertexIndex, adjVertexID.Data(), polygonNormals);
//				if (collapseCost < vertexEdgeCollapseCosts[vertexIndex])
//				{
//					vertexEdgeCollapseCosts[vertexIndex] = collapseCost;
//					vertexEdgeCollapseTargets[vertexIndex] = adjVertexID;
//				}
//			}
//		}
//
//		// Start to collapse edge.
//		auto GetMinCollapseCostVertexIndex = [&vertexEdgeCollapseCosts, &mesh]()
//		{
//			float minCost = FLT_MAX;
//			uint32_t minCostVertexIndex = cd::VertexID::InvalidID;
//			for (uint32_t vertexIndex = 0U, vertexCount = mesh.GetVertexCount(); vertexIndex < vertexCount; ++vertexIndex)
//			{
//				if (vertexEdgeCollapseCosts[vertexIndex] < minCost)
//				{
//					minCost = vertexEdgeCollapseCosts[vertexIndex];
//					minCostVertexIndex = vertexIndex;
//				}
//			}
//
//			return minCostVertexIndex;
//		};
//
//		for (int collapseIndex = originVertexCount - 1; collapseIndex >= 0; --collapseIndex)
//		{
//			uint32_t v0 = GetMinCollapseCostVertexIndex();
//			uint32_t v1 = vertexEdgeCollapseTargets[v0].Data();
//
//			printf("Collapse Edge (%u, %u)\n", v0, v1);
//			CollapseEdge(mesh, v0, v1);
//			break;
//		}
//	}
//
//	void CollapseEdge(cd::Mesh& mesh, uint32_t v0, uint32_t v1)
//	{
//		assert(v0 != cd::VertexID::InvalidID);
//		assert(v1 != cd::VertexID::InvalidID);
//
//		// Delete polygons
//		{
//			uint32_t adjPolygonCount = mesh.GetVertexAdjacentPolygonCount(v0);
//			const cd::PolygonIDArray& adjPolygons = mesh.GetVertexAdjacentPolygonArray(v0);
//			for (uint32_t polygonIndex = 0U; polygonIndex < adjPolygonCount; ++polygonIndex)
//			{
//				cd::PolygonID adjacentPolygonID = adjPolygons[polygonIndex];
//				const cd::Polygon& adjacentPolygon = mesh.GetPolygon(adjacentPolygonID.Data());
//				if (adjacentPolygon.Contains(cd::VertexID(v1)))
//				{
//					// Delete Edge (v0, v1) adjacent polygons
//					mesh.RemovePolygonData(adjacentPolygonID);
//
//					// 1. Loop through polygon's every vertex which maintains adjacent polygon id.
//					{
//						const auto& polygons = mesh.GetPolygons();
//						for (size_t vertexIndex = 0U; vertexIndex < adjacentPolygon.Size; ++vertexIndex)
//						{
//							auto polygonVertexID = adjacentPolygon[vertexIndex];
//							auto& adjPolygons = mesh.GetVertexAdjacentPolygonArray(polygonVertexID.Data());
//							adjPolygons.erase(std::remove(adjPolygons.begin(), adjPolygons.end(), polygonVertexID.Data()), adjPolygons.end());
//						}
//					}
//				}
//			}
//		}
//
//		// Replace v1 with v0.
//		{
//			// 1. Loop through v1's adjacent polygons and maintain polygon's adjacent vertex ids.
//			for (auto v1AdjPolygonID : mesh.GetVertexAdjacentPolygonArray(v1))
//			{
//				auto& v1AdjPolygon = mesh.GetPolygon(v1);
//				for (size_t vertexIndex = 0U; vertexIndex < v1AdjPolygon.Size; ++vertexIndex)
//				{
//					auto polygonVertexID = v1AdjPolygon[vertexIndex];
//					if (polygonVertexID.Data() == v1)
//					{
//						v1AdjPolygon[vertexIndex] = cd::VertexID(v0);
//					}
//				}
//			}
//
//			// 2. Loop through v1's adjacent vertices and maintain vertex adjacent vertex ids.
//			for (auto v1AdjVertexID : mesh.GetVertexAdjacentVertexArray(v1))
//			{
//				auto& v1AdjVertexAdjVertexIDs = mesh.GetVertexAdjacentVertexArray(v1AdjVertexID.Data());
//				for (size_t vertexIndex = 0U; vertexIndex < v1AdjVertexAdjVertexIDs.size(); ++vertexIndex)
//				{
//					auto v1AdjVertexAdjVertexID = v1AdjVertexAdjVertexIDs[vertexIndex];
//					if (v1AdjVertexAdjVertexID.Data() == v1)
//					{
//						v1AdjVertexAdjVertexIDs[vertexIndex] = cd::VertexID(v0);
//					}
//				}
//			}
//			
//			// Replace v1 with v0.
//			mesh.SwapVertexData(cd::VertexID(v0), cd::VertexID(v1));
//			mesh.RemoveVertexData(cd::VertexID(v1));
//		}
//	}
//
//	float CalculateEdgeCollapseCost(const cd::Mesh& mesh, uint32_t v0, uint32_t v1, const std::vector<cd::Direction>& polygonNormals)
//	{
//		cd::VertexID v1ID(v1);
//		cd::PolygonIDArray sharedPolygons;
//		for (cd::PolygonID polygonID : mesh.GetVertexAdjacentPolygonArray(v0))
//		{
//			const cd::Polygon& polygon = mesh.GetPolygon(polygonID.Data());
//			if (polygon.Contains(v1ID))
//			{
//				sharedPolygons.push_back(polygonID);
//			}
//		}
//
//		float curvature = 0.0f;
//		for (cd::PolygonID polygonID : mesh.GetVertexAdjacentPolygonArray(v0))
//		{
//			float minCurvature = 1.0f;
//			const cd::Direction& v0FaceNormal = polygonNormals[polygonID.Data()];
//			for (cd::PolygonID sharedPolygonID : sharedPolygons)
//			{
//				const cd::Direction& sharedFaceNormal = polygonNormals[sharedPolygonID.Data()];
//				float faceNormalDot = v0FaceNormal.Dot(sharedFaceNormal);
//				float t = (1 - faceNormalDot) * 0.5f;
//				if (t < minCurvature)
//				{
//					minCurvature = t;
//				}
//			}
//			
//			if (minCurvature > curvature)
//			{
//				curvature = minCurvature;
//			}
//		}
//
//		float edgeLength = (mesh.GetVertexPosition(v0) - mesh.GetVertexPosition(v1)).Length();
//		return edgeLength * curvature;
//	}
//
//private:
//	std::string m_filePath;
//};

}