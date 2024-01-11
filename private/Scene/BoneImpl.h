#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class BoneImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Bone);

	bool IsRootBone() const { return !GetParentID().IsValid(); }

	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, ParentID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, SkeletonID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, LimbLength);
	IMPLEMENT_STRING_TYPE_APIS(Bone, Name);
	IMPLEMENT_COMPLEX_TYPE_APIS(Bone, LimbSize);
	IMPLEMENT_COMPLEX_TYPE_APIS(Bone, Offset);
	IMPLEMENT_COMPLEX_TYPE_APIS(Bone, Transform);
	IMPLEMENT_VECTOR_TYPE_APIS(Bone, ChildID);

	template<bool SwapBytesOrder>
	BoneImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t boneChildIDCount;

		inputArchive >> GetID().Data() >> GetParentID().Data() >> GetName() >> boneChildIDCount;
		SetChildIDCount(boneChildIDCount);
		inputArchive.ImportBuffer(GetChildIDs().data());
		inputArchive >> GetOffset() >> GetTransform();

		return *this;
	}

	template<bool SwapBytesOrder>
	const BoneImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetParentID().Data() << GetName() << GetChildIDCount();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive << GetOffset() << GetTransform();

		return *this;
	}
};

}