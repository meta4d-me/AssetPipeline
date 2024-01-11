#pragma once

namespace cdtools
{

enum class FbxProducerOptions
{
	// Preprocess
	Triangulate,

	// Scene objects
	ImportAnimation,
	ImportBlendShape,
	ImportMaterial,
	ImportTexture,
	ImportLight,
	ImportSkeleton,
	ImportSkeletalMesh,
	ImportStaticMesh,
};

}