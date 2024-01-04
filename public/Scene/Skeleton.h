#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

namespace cd
{

class SkeletonImpl;

class CORE_API Skeleton final
{
public:
	DECLARE_SCENE_CLASS(Skeleton);
	EXPORT_SIMPLE_TYPE_APIS(Skeleton, ID);
	EXPORT_STRING_TYPE_APIS(Skeleton, Name);
	EXPORT_VECTOR_TYPE_APIS(Skeleton, RootBoneID);
	EXPORT_VECTOR_TYPE_APIS(Skeleton, BoneID);
};

}