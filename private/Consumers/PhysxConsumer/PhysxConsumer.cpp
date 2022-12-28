#include "PhysxConsumer.h"

#include "Scene/SceneDatabase.h"

#include <PxPhysicsAPI.h>

// C/C++
#include <cassert>
#include <iostream>
#include <vector>

namespace cdtools
{

class PhysxErrorLog : public physx::PxErrorCallback
{
public:
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		std::cerr << message << std::endl << "file: " << file << ", line: " << line << std::endl;
	}
};

void PhysxConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	physx::PxDefaultAllocator physxDefaultAllocator;
	PhysxErrorLog physxErrorLog;
	physx::PxFoundation* pSDKFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, physxDefaultAllocator, physxErrorLog);
	assert(pSDKFoundation && "Failed to init physx foundation.");

	physx::PxTolerancesScale cookingScale;
	physx::PxCooking* pSDKCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pSDKFoundation, physx::PxCookingParams(cookingScale));
	assert(pSDKCooking && "Failed to init physx cooking.");

	std::ofstream fout(m_filePath, std::ios::out | std::ios::binary);

	uint8_t isConvexMesh = IsConvexServiceActive() ? 1 : 0;
	fout.write(reinterpret_cast<char*>(&isConvexMesh), sizeof(isConvexMesh));

	uint32_t meshCount = pSceneDatabase->GetMeshCount();
	fout.write(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
	{
		const Mesh& mesh = pSceneDatabase->GetMesh(meshIndex);
		uint32_t vertexCount = mesh.GetVertexCount();

		// Initialize collision mesh data for physx cooking service. 
		std::vector<physx::PxVec3> physxMeshVertices;
		physxMeshVertices.reserve(vertexCount);
		for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
		{
			const Point& position = mesh.GetVertexPosition(vertexIndex);
			physxMeshVertices.push_back(physx::PxVec3(position.x(), position.y(), position.z()));
		}

		physx::PxDefaultMemoryOutputStream collisionMeshBuffer;
		if (IsConvexServiceActive())
		{
			physx::PxConvexMeshDesc convexMeshDesc;
			convexMeshDesc.points.count = static_cast<uint32_t>(physxMeshVertices.size());
			convexMeshDesc.points.data = physxMeshVertices.data();
			convexMeshDesc.points.stride = sizeof(physx::PxVec3);
			convexMeshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
			bool cookResult = pSDKCooking->cookConvexMesh(convexMeshDesc, collisionMeshBuffer);
			assert(cookResult && "Failed to cook convex mesh.");
		}
		else
		{
			physx::PxTriangleMeshDesc triangleMeshDesc;
			triangleMeshDesc.points.count = static_cast<uint32_t>(physxMeshVertices.size());
			triangleMeshDesc.points.data = physxMeshVertices.data();
			triangleMeshDesc.points.stride = sizeof(physx::PxVec3);
			bool cookResult = pSDKCooking->cookTriangleMesh(triangleMeshDesc, collisionMeshBuffer);
			assert(cookResult && "Failed to cook triangle mesh.");
		}

		uint32_t collisionMeshBufferSize = collisionMeshBuffer.getSize();
		fout.write(reinterpret_cast<char*>(&collisionMeshBufferSize), sizeof(collisionMeshBufferSize));
		fout.write(reinterpret_cast<char*>(collisionMeshBuffer.getData()), collisionMeshBufferSize);
	}

	fout.close();
}

}