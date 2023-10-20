#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/ObjectID.h"
#include "Scene/Track.h"

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

	IMPLEMENT_ID_APIS(AnimationID, m_id);
	IMPLEMENT_NAME_APIS(m_name);

	void SetDuration(float duration) { m_duration = duration; }
	float GetDuration() const { return m_duration; }

	void SetTicksPerSecond(float ticksPerSecond) { m_ticksPerSecond = ticksPerSecond; }
	float GetTicksPerSecnod() const { return m_ticksPerSecond; }

	void AddBoneTrackID(uint32_t trackID) { m_boneTrackIDs.push_back(TrackID(trackID)); }
	uint32_t GetBoneTrackCount() const { return static_cast<uint32_t>(m_boneTrackIDs.size()); }
	std::vector<TrackID>& GetBoneTrackIDs() { return m_boneTrackIDs; }
	const std::vector<TrackID>& GetBoneTrackIDs() const { return m_boneTrackIDs; }

	template<bool SwapBytesOrder>
	AnimationImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t animationID;
		std::string animationName;
		float duration;
		float ticksPerSecond;
		uint32_t boneTrackCount;

		inputArchive >> animationID >> animationName >> duration >> ticksPerSecond >> boneTrackCount;

		Init(AnimationID(animationID), cd::MoveTemp(animationName));
		SetDuration(duration);
		SetTicksPerSecond(ticksPerSecond);

		m_boneTrackIDs.resize(boneTrackCount);
		inputArchive.ImportBuffer(m_boneTrackIDs.data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const AnimationImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetDuration() << GetTicksPerSecnod() << GetBoneTrackCount();
		outputArchive.ExportBuffer(GetBoneTrackIDs().data(), GetBoneTrackIDs().size());

		return *this;
	}

private:
	AnimationID m_id;
	float m_duration;
	float m_ticksPerSecond;

	std::string m_name;
	std::vector<TrackID> m_boneTrackIDs;
};

}