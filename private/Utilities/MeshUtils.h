#pragma once

#include "Math/AABB.hpp"
#include "Math/VectorDerived.hpp"
#include "Scene/Mesh.h"

namespace cdtools
{
	cd::AABB CalculateAABB(const cd::Mesh& mesh)
	{
		cd::Point minPoint;
		cd::Point maxPoint;
		const std::vector<cd::Point>& meshPoints = mesh.GetVertexPositions();
		for (uint32_t i = 0; i < meshPoints.size(); ++i)
		{
			const cd::Point& current = meshPoints[i];
			minPoint[0] = std::min(current[0], minPoint[0]);
			minPoint[1] = std::min(current[1], minPoint[1]);
			minPoint[2] = std::min(current[2], minPoint[2]);
			maxPoint[0] = std::max(current[0], maxPoint[0]);
			maxPoint[1] = std::max(current[1], maxPoint[1]);
			maxPoint[2] = std::max(current[2], maxPoint[2]);
		}
		return cd::AABB(minPoint, maxPoint);
	}
}