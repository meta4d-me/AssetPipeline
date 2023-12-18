#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

namespace cd
{

class SkinImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Skin);

	IMPLEMENT_SIMPLE_TYPE_APIS(Skin, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Skin, SkeletonID);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexInfluenceBoneID);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneIndex);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneWeight);
	IMPLEMENT_STRING_TYPE_APIS(Skin, Name);
	
	template<bool SwapBytesOrder>
	SkinImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		inputArchive >> GetID().Data() >> GetSkeletonID().Data() >> GetName();
		return *this;
	}

	template<bool SwapBytesOrder>
	const SkinImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetSkeletonID().Data() << GetName();
		return *this;
	}
};

}