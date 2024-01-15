#pragma once

namespace cdtools
{

enum class FbxConsumerOptions
{
	// File Format
	SaveAsASCII,

	// Scene objects
	ExportAnimation,
	ExportBlendShape,
	ExportMaterial,
	ExportTexture,
	ExportLight,
	ExportSkeleton,
	ExportSkeletalMesh,
	ExportStaticMesh,
};

}