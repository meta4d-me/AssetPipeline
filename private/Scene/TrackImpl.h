#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class TrackImpl final
{
public:
	TrackImpl() = delete;
	template<bool SwapBytesOrder>
	explicit TrackImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit TrackImpl(TrackID id, std::string name);
	TrackImpl(const TrackImpl&) = default;
	TrackImpl& operator=(const TrackImpl&) = default;
	TrackImpl(TrackImpl&&) = default;
	TrackImpl& operator=(TrackImpl&&) = default;
	~TrackImpl() = default;

	void Init(TrackID id, std::string name);

	IMPLEMENT_ID_APIS(TrackID, m_id);
	IMPLEMENT_NAME_APIS(m_name);
	IMPLEMENT_VECTOR_DATA_APIS(TranslationKey, m_translationKeys);
	IMPLEMENT_VECTOR_DATA_APIS(RotationKey, m_rotationKeys);
	IMPLEMENT_VECTOR_DATA_APIS(ScaleKey, m_scaleKeys);

	template<bool SwapBytesOrder>
	TrackImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t trackID;
		std::string trackName;
		uint32_t translationKeyCount;
		uint32_t rotationKeyCount;
		uint32_t scaleKeyCount;

		inputArchive >> trackID >> trackName
			>> translationKeyCount >> rotationKeyCount >> scaleKeyCount;

		Init(TrackID(trackID), cd::MoveTemp(trackName));

		SetTranslationKeyCount(translationKeyCount);
		inputArchive.ImportBuffer(GetTranslationKeys().data());

		SetRotationKeyCount(rotationKeyCount);
		inputArchive.ImportBuffer(GetRotationKeys().data());

		SetScaleKeyCount(scaleKeyCount);
		inputArchive.ImportBuffer(GetScaleKeys().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TrackImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName()
			<< GetTranslationKeyCount() << GetRotationKeyCount() << GetScaleKeyCount();

		outputArchive.ExportBuffer(GetTranslationKeys().data(), GetTranslationKeys().size());
		outputArchive.ExportBuffer(GetRotationKeys().data(), GetRotationKeys().size());
		outputArchive.ExportBuffer(GetScaleKeys().data(), GetScaleKeys().size());

		return *this;
	}

private:
	TrackID m_id;
	std::string m_name;

	std::vector<TranslationKey> m_translationKeys;
	std::vector<RotationKey> m_rotationKeys;
	std::vector<ScaleKey> m_scaleKeys;
};

}