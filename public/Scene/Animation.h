#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class AnimationImpl;

class CORE_API Animation final
{
public:
	DECLARE_SCENE_CLASS(Animation);
	explicit Animation(AnimationID id, std::string name);
	void Init(AnimationID id, std::string name);

	EXPORT_SIMPLE_TYPE_APIS(Animation, ID);
	EXPORT_SIMPLE_TYPE_APIS(Animation, Duration);
	EXPORT_SIMPLE_TYPE_APIS(Animation, TicksPerSecond);
	EXPORT_VECTOR_TYPE_APIS(Animation, BoneTrackID);
	EXPORT_STRING_TYPE_APIS(Animation, Name);
};

}