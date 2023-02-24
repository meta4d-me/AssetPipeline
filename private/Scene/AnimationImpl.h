#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class AnimationImpl final
{
public:
	AnimationImpl() = delete;
	template<bool SwapBytesOrder>
	explicit AnimationImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit AnimationImpl(AnimationID id, std::string name);
	AnimationImpl(const AnimationImpl&) = default;
	AnimationImpl& operator=(const AnimationImpl&) = default;
	AnimationImpl(AnimationImpl&&) = default;
	AnimationImpl& operator=(AnimationImpl&&) = default;
	~AnimationImpl() = default;

	void Init(AnimationID id, std::string name);

	const AnimationID& GetID() const { return m_id; }

	void SetName(std::string name) { m_name = cd::MoveTemp(name); }
	const std::string& GetName() const { return m_name; }

	template<bool SwapBytesOrder>
	AnimationImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t animationID;
		std::string animationName;

		inputArchive >> animationID >> animationName;

		Init(AnimationID(animationID), cd::MoveTemp(animationName));

		return *this;
	}

	template<bool SwapBytesOrder>
	const AnimationImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName();

		return *this;
	}

private:
	AnimationID m_id;
	std::string m_name;
};

}