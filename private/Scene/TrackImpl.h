#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class TrackImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Track);

	explicit TrackImpl(TrackID id, std::string name);
	void Init(TrackID id, std::string name);

	IMPLEMENT_SIMPLE_TYPE_APIS(Track, ID);
	IMPLEMENT_STRING_TYPE_APIS(Track, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Track, TranslationKey);
	IMPLEMENT_VECTOR_TYPE_APIS(Track, RotationKey);
	IMPLEMENT_VECTOR_TYPE_APIS(Track, ScaleKey);

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
};

}