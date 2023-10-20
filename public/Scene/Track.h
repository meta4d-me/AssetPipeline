#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class TrackImpl;

class CORE_API Track final
{
public:
	static const char* GetClassName() { return "Track"; }

public:
	Track() = delete;
	explicit Track(InputArchive& inputArchive);
	explicit Track(InputArchiveSwapBytes& inputArchive);
	explicit Track(TrackID id, std::string name);
	Track(const Track&) = delete;
	Track& operator=(const Track&) = delete;
	Track(Track&&);
	Track& operator=(Track&&);
	~Track();

	void Init(TrackID id, std::string name);

	EXPORT_OBJECT_ID_APIS(TrackID);
	EXPORT_NAME_APIS();

	EXPORT_VECTOR_DATA_APIS(TranslationKey);
	EXPORT_VECTOR_DATA_APIS(RotationKey);
	EXPORT_VECTOR_DATA_APIS(ScaleKey);

	Track& operator<<(InputArchive& inputArchive);
	Track& operator<<(InputArchiveSwapBytes& inputArchive);
	const Track& operator>>(OutputArchive& outputArchive) const;
	const Track& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TrackImpl* m_pTrackImpl = nullptr;
};

}