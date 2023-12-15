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
	static const char* GetClassName() { return "Animation"; }

public:
	Animation() = delete;
	explicit Animation(InputArchive& inputArchive);
	explicit Animation(InputArchiveSwapBytes& inputArchive);
	explicit Animation(AnimationID id, std::string name);
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation(Animation&&);
	Animation& operator=(Animation&&);
	~Animation();

	void Init(AnimationID id, std::string name);

	EXPORT_SIMPLE_TYPE_APIS(Animation, ID);
	EXPORT_SIMPLE_TYPE_APIS(Animation, Duration);
	EXPORT_SIMPLE_TYPE_APIS(Animation, TicksPerSecond);
	EXPORT_VECTOR_TYPE_APIS(Animation, BoneTrackID);
	EXPORT_STRING_TYPE_APIS(Animation, Name);

	Animation& operator<<(InputArchive& inputArchive);
	Animation& operator<<(InputArchiveSwapBytes& inputArchive);
	const Animation& operator>>(OutputArchive& outputArchive) const;
	const Animation& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	AnimationImpl* m_pAnimationImpl = nullptr;
};

}