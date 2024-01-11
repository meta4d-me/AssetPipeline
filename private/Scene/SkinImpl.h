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
	IMPLEMENT_SIMPLE_TYPE_APIS(Skin, MaxVertexInfluenceCount);
	IMPLEMENT_STRING_TYPE_APIS(Skin, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, InfluenceBoneName);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneNameArray);
	IMPLEMENT_VECTOR_TYPE_APIS(Skin, VertexBoneWeightArray);
	
	template<bool SwapBytesOrder>
	SkinImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t influenceBoneCount;
		uint32_t influenceVertexCount;
		inputArchive >> GetID().Data() >> GetMeshID().Data() >> GetName() >> GetMaxVertexInfluenceCount()
			>> influenceBoneCount >> influenceVertexCount;
		
		SetInfluenceBoneNameCount(influenceBoneCount);
		for (uint32_t influenceBoneIndex = 0U; influenceBoneIndex < influenceBoneCount; ++influenceBoneIndex)
		{
			inputArchive.ImportBuffer(GetInfluenceBoneName(influenceBoneIndex).data());
		}

		SetVertexBoneNameArrayCount(influenceVertexCount);
		for (uint32_t influenceVertexIndex = 0U; influenceVertexIndex < influenceVertexCount; ++influenceVertexIndex)
		{
			auto& vertexBoneNameArray = GetVertexBoneNameArray(influenceVertexIndex);
			vertexBoneNameArray.resize(GetMaxVertexInfluenceCount());
			inputArchive.ImportBuffer(vertexBoneNameArray.data());
		}

		SetVertexBoneWeightArrayCount(influenceVertexCount);
		for (uint32_t influenceVertexIndex = 0U; influenceVertexIndex < influenceVertexCount; ++influenceVertexIndex)
		{
			auto& vertexWeightArray = GetVertexBoneWeightArray(influenceVertexIndex);
			vertexWeightArray.resize(GetMaxVertexInfluenceCount());
			inputArchive.ImportBuffer(vertexWeightArray.data());
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const SkinImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetMeshID().Data() << GetName() << GetMaxVertexInfluenceCount()
			<< GetInfluenceBoneNameCount() << GetVertexBoneNameArrayCount();

		for (uint32_t influenceBoneIndex = 0U; influenceBoneIndex < GetInfluenceBoneNameCount(); ++influenceBoneIndex)
		{
			outputArchive.ExportBuffer(GetInfluenceBoneName(influenceBoneIndex).data(), GetInfluenceBoneName(influenceBoneIndex).size());
		}

		for (uint32_t influenceVertexIndex = 0U; influenceVertexIndex < GetVertexBoneNameArrayCount(); ++influenceVertexIndex)
		{
			const auto& vertexBoneNameArray = GetVertexBoneNameArray(influenceVertexIndex);
			outputArchive.ExportBuffer(vertexBoneNameArray.data(), vertexBoneNameArray.size());
		}

		for (uint32_t influenceVertexIndex = 0U; influenceVertexIndex < GetVertexBoneNameArrayCount(); ++influenceVertexIndex)
		{
			const auto& vertexWeightArray = GetVertexBoneWeightArray(influenceVertexIndex);
			outputArchive.ExportBuffer(vertexWeightArray.data(), vertexWeightArray.size());
		}

		return *this;
	}
};

}