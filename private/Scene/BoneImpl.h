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

	explicit BoneImpl(BoneID id, std::string name);
	void Init(BoneID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Bone, ParentID);
	IMPLEMENT_COMPLEX_TYPE_APIS(Bone, Offset);
	IMPLEMENT_COMPLEX_TYPE_APIS(Bone, Transform);
	IMPLEMENT_VECTOR_TYPE_APIS(Bone, ChildID);
	IMPLEMENT_STRING_TYPE_APIS(Bone, Name);

	template<bool SwapBytesOrder>
	BoneImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t boneID;
		std::string boneName;
		uint32_t boneParentID;
		uint32_t boneChildIDCount;

		inputArchive >> boneID >> boneName
			>> boneParentID >> boneChildIDCount;

		Init(BoneID(boneID), cd::MoveTemp(boneName));
		SetParentID(BoneID(boneParentID));

		GetChildIDs().resize(boneChildIDCount);
		inputArchive.ImportBuffer(GetChildIDs().data());

		inputArchive >> GetOffset() >> GetTransform();

		return *this;
	}

	template<bool SwapBytesOrder>
	const BoneImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetParentID().Data() << GetChildIDCount();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive << GetOffset() << GetTransform();

		return *this;
	}
};

}