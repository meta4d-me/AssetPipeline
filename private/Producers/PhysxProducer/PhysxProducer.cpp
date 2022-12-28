#include "PhysxProducer.h"
#include "Scene/SceneDatabase.h"

#include <PxPhysicsAPI.h>

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

void PhysxProducer::Execute(SceneDatabase* pSceneDatabase)
{
	physx::PxDefaultAllocator physxDefaultAllocator;
	PhysxErrorLog physxErrorLog;
	physx::PxFoundation* pSDKFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, physxDefaultAllocator, physxErrorLog);
	assert(pSDKFoundation && "Failed to init physx foundation.");

	physx::PxTolerancesScale physxScale;
	physx::PxPhysics* pPhysxInstance = PxCreatePhysics(PX_PHYSICS_VERSION, *pSDKFoundation, physxScale);
	assert(pSDKFoundation && "Failed to create physics instance.");

	std::ifstream fin(m_filePath, std::ios::in | std::ios::binary);

	uint8_t isConvexMesh;
	fin.read(reinterpret_cast<char*>(&isConvexMesh), sizeof(isConvexMesh));

	uint32_t meshCount = 0U;
	fin.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));

	for (uint32_t meshIndex = 0U; meshIndex < meshCount; ++meshIndex)
	{
		uint32_t meshBufferSize = 0U;
		fin.read(reinterpret_cast<char*>(&meshBufferSize), sizeof(meshBufferSize));

		uint8_t* pMeshBuffer = new uint8_t[meshBufferSize];
		fin.read(reinterpret_cast<char*>(pMeshBuffer), meshBufferSize);

		physx::PxDefaultMemoryInputData inputData(pMeshBuffer, meshBufferSize);
		if(1 == isConvexMesh)
		{
			physx::PxConvexMesh* pConvexMesh = pPhysxInstance->createConvexMesh(inputData);
			assert(pConvexMesh && "Failed to create convex mesh.");
		}
		else
		{
			physx::PxTriangleMesh* pTriangleMesh = pPhysxInstance->createTriangleMesh(inputData);
			assert(pTriangleMesh && "Failed to create triangle mesh.");
		}
	}

	fin.close();
}

}