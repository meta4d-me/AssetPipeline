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
	
	template<bool SwapBytesOrder>
	SkeletonImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		return *this;
	}

	template<bool SwapBytesOrder>
	const SkeletonImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		return *this;
	}
};

}