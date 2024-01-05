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
	IMPLEMENT_SIMPLE_TYPE_APIS(Skin, MeshID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Skin, SkeletonID);
	IMPLEMENT_STRING_TYPE_APIS(Skin, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexInfluenceBoneName);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneName);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneWeight);
	
	template<bool SwapBytesOrder>
	SkinImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t influenceBoneCount;
		uint32_t influenceVertexCount;
		inputArchive >> GetID().Data() >> GetMeshID().Data() >> GetName() >> influenceBoneCount >> influenceVertexCount;
		
		SetVertexInfluenceBoneNameCount(influenceBoneCount);
		SetVertexBoneNameCount(influenceVertexCount);
		for (uint32_t influenceVertexIndex = 0U; influenceVertexIndex < influenceVertexCount; ++influenceVertexIndex)
		{
			inputArchive.ImportBuffer(GetVertexBoneName(influenceVertexIndex).data(), GetVertexBoneName(influenceVertexIndex).size());
		}

		SetVertexBoneWeightCount(influenceVertexCount);
		inputArchive.ImportBuffer(GetVertexBoneWeights().data(), GetVertexBoneWeights().size());

		return *this;
	}

	template<bool SwapBytesOrder>
	const SkinImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetMeshID().Data() << GetName()
			<< GetVertexInfluenceBoneNameCount() << GetVertexBoneNameCount();
		return *this;
	}
};

}