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

	const TrackID& GetID() const;

	void SetName(std::string name);
	const char* GetName() const;

	void SetTranslationKeyCount(uint32_t keyCount);
	uint32_t GetTranslationKeyCount() const;
	void SetTranslationKeys(std::vector<TranslationKey> keys);
	std::vector<TranslationKey>& GetTranslationKeys();
	const std::vector<TranslationKey>& GetTranslationKeys() const;

	void SetRotationKeyCount(uint32_t keyCount);
	uint32_t GetRotationKeyCount() const;
	void SetRotationKeys(std::vector<RotationKey> keys);
	std::vector<RotationKey>& GetRotationKeys();
	const std::vector<RotationKey>& GetRotationKeys() const;

	void SetScaleKeyCount(uint32_t keyCount);
	uint32_t GetScaleKeyCount() const;
	void SetScaleKeys(std::vector<ScaleKey> keys);
	std::vector<ScaleKey>& GetScaleKeys();
	const std::vector<ScaleKey>& GetScaleKeys() const;

	Track& operator<<(InputArchive& inputArchive);
	Track& operator<<(InputArchiveSwapBytes& inputArchive);
	const Track& operator>>(OutputArchive& outputArchive) const;
	const Track& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	TrackImpl* m_pTrackImpl = nullptr;
};

}