#pragma once

namespace cdtools
{

enum class GenericProducerOptions
{
	GenerateBoundingBox,
	FlattenTransformHierarchy,
	TriangulateModel,
	GenerateTangentSpace,
	CleanUnusedObjects,
	OnlyTransformAnimationKey,
	OptimizeMeshBufferCacheHitRate
};

}