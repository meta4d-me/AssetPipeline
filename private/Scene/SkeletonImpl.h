#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

namespace cd
{

class SkeletonImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Skeleton);

	IMPLEMENT_SIMPLE_TYPE_APIS(Skeleton, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Skeleton, RootBoneID);
	IMPLEMENT_STRING_TYPE_APIS(Skeleton, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Skeleton, BoneID);
	
	template<bool SwapBytesOrder>
	SkeletonImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t boneIDCount;
		inputArchive >> GetID().Data() >> GetRootBoneID().Data() >> GetName() >> boneIDCount;
		SetBoneIDCount(boneIDCount);
		inputArchive.ImportBuffer(GetBoneIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const SkeletonImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetRootBoneID().Data() << GetName() << GetBoneIDCount();
		outputArchive.ExportBuffer(GetBoneIDs().data(), GetBoneIDs().size());

		return *this;
	}
};

}