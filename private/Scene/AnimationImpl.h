#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/Track.h"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class AnimationImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Animation);

	explicit AnimationImpl(AnimationID id, std::string name);
	void Init(AnimationID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(Animation, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Animation, Duration);
	IMPLEMENT_SIMPLE_TYPE_APIS(Animation, TicksPerSecond);
	IMPLEMENT_STRING_TYPE_APIS(Animation, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Animation, BoneTrackID);

	template<bool SwapBytesOrder>
	AnimationImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t animationID;
		std::string animationName;
		inputArchive >> animationID >> animationName;
		Init(AnimationID(animationID), cd::MoveTemp(animationName));

		uint32_t boneTrackCount;
		inputArchive >> GetDuration() >> GetTicksPerSecond() >> boneTrackCount;
		SetBoneTrackIDCount(boneTrackCount);
		inputArchive.ImportBuffer(GetBoneTrackIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const AnimationImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetDuration() << GetTicksPerSecond() << GetBoneTrackIDCount();
		outputArchive.ExportBuffer(GetBoneTrackIDs().data(), GetBoneTrackIDs().size());

		return *this;
	}
};

}