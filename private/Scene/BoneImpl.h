#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class BoneImpl final
{
public:
	BoneImpl() = delete;
	template<bool SwapBytesOrder>
	explicit BoneImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}
	explicit BoneImpl(BoneID id, std::string name);
	BoneImpl(const BoneImpl&) = default;
	BoneImpl& operator=(const BoneImpl&) = default;
	BoneImpl(BoneImpl&&) = default;
	BoneImpl& operator=(BoneImpl&&) = default;
	~BoneImpl() = default;

	void Init(BoneID id, std::string name);

	void SetID(uint32_t id) { m_id = BoneID(id); }
	const BoneID& GetID() const { return m_id; }

	void SetName(std::string name) { m_name = cd::MoveTemp(name); }
	std::string& GetName() { return m_name; }
	const std::string& GetName() const { return m_name; }

	void SetParentID(uint32_t parentID) { m_parentID.Set(parentID); }
	BoneID& GetParentID() { return m_parentID; }
	const BoneID& GetParentID() const { return m_parentID; }

	void AddChildID(uint32_t childID) { m_childIDs.push_back(BoneID(childID)); }
	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_childIDs.size()); }
	std::vector<BoneID>& GetChildIDs() { return m_childIDs; }
	const std::vector<BoneID>& GetChildIDs() const { return m_childIDs; }

	template<bool SwapBytesOrder>
	BoneImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t boneID;
		uint32_t boneParentID;
		std::string boneName;

		inputArchive >> boneID >> boneName >> boneParentID;

		Init(BoneID(boneID), cd::MoveTemp(boneName));

		SetParentID(boneParentID);
		inputArchive.ImportBuffer(GetChildIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const BoneImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());

		return *this;
	}

private:
	BoneID m_id;
	BoneID m_parentID;
	std::vector<BoneID> m_childIDs;

	std::string m_name;
};

}