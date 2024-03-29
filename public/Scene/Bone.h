#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class BoneImpl;

class CORE_API Bone final
{
public:
	DECLARE_SCENE_CLASS(Bone);

	EXPORT_SIMPLE_TYPE_APIS(Bone, ID);
	EXPORT_SIMPLE_TYPE_APIS(Bone, ParentID);
	EXPORT_SIMPLE_TYPE_APIS(Bone, SkeletonID);
	EXPORT_SIMPLE_TYPE_APIS(Bone, LimbLength);
	EXPORT_STRING_TYPE_APIS(Bone, Name);
	EXPORT_COMPLEX_TYPE_APIS(Bone, LimbSize);
	EXPORT_COMPLEX_TYPE_APIS(Bone, Offset);
	EXPORT_COMPLEX_TYPE_APIS(Bone, Transform);
	EXPORT_VECTOR_TYPE_APIS(Bone, ChildID);

	bool IsRootBone() const;
};

}