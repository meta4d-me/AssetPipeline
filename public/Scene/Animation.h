#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class AnimationImpl;

class CORE_API Animation final
{
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

	const AnimationID& GetID() const;

	void SetName(std::string name);
	const char* GetName() const;

	Animation& operator<<(InputArchive& inputArchive);
	Animation& operator<<(InputArchiveSwapBytes& inputArchive);
	const Animation& operator>>(OutputArchive& outputArchive) const;
	const Animation& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	AnimationImpl* m_pAnimationImpl = nullptr;
};

}