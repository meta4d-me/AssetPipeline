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

	const TrackID& GetID() const { return m_id; }

	void SetName(std::string name) { m_name = cd::MoveTemp(name); }
	const std::string& GetName() const { return m_name; }

	void SetTranslationKeyCount(uint32_t keyCount) { m_translationKeys.resize(keyCount); }
	uint32_t GetTranslationKeyCount() const { return static_cast<uint32_t>(m_translationKeys.size()); }
	void SetTranslationKeys(std::vector<TranslationKey> keys) { m_translationKeys = MoveTemp(keys); }
	std::vector<TranslationKey>& GetTranslationKeys() { return m_translationKeys; }
	const std::vector<TranslationKey>& GetTranslationKeys() const { return m_translationKeys; }

	void SetRotationKeyCount(uint32_t keyCount) { m_rotationKeys.resize(keyCount); }
	uint32_t GetRotationKeyCount() const { return static_cast<uint32_t>(m_rotationKeys.size()); }
	void SetRotationKeys(std::vector<RotationKey> keys) { m_rotationKeys = MoveTemp(keys); }
	std::vector<RotationKey>& GetRotationKeys() { return m_rotationKeys; }
	const std::vector<RotationKey>& GetRotationKeys() const { return m_rotationKeys; }

	void SetScaleKeyCount(uint32_t keyCount) { m_scaleKeys.resize(keyCount); }
	uint32_t GetScaleKeyCount() const { return static_cast<uint32_t>(m_scaleKeys.size()); }
	void SetScaleKeys(std::vector<ScaleKey> keys) { m_scaleKeys = MoveTemp(keys); }
	std::vector<ScaleKey>& GetScaleKeys() { return m_scaleKeys; }
	const std::vector<ScaleKey>& GetScaleKeys() const { return m_scaleKeys; }

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