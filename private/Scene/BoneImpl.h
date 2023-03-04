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

	void SetName(std::string name) { m_name = MoveTemp(name); }
	std::string& GetName() { return m_name; }
	const std::string& GetName() const { return m_name; }

	void SetParentID(uint32_t parentID) { m_parentID.Set(parentID); }
	BoneID& GetParentID() { return m_parentID; }
	const BoneID& GetParentID() const { return m_parentID; }

	void AddChildID(uint32_t childID) { m_childIDs.push_back(BoneID(childID)); }
	uint32_t GetChildCount() const { return static_cast<uint32_t>(m_childIDs.size()); }
	std::vector<BoneID>& GetChildIDs() { return m_childIDs; }
	const std::vector<BoneID>& GetChildIDs() const { return m_childIDs; }

	void SetOffset(Matrix4x4 offset) { m_offset = MoveTemp(offset); }
	Matrix4x4& GetOffset() { return m_offset; }
	const Matrix4x4& GetOffset() const { return m_offset; }

	void SetTransform(Transform transform) { m_transform = MoveTemp(transform); }
	Transform& GetTransform() { return m_transform; }
	const Transform& GetTransform() const { return m_transform; }

	template<bool SwapBytesOrder>
	BoneImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t boneID;
		std::string boneName;
		uint32_t boneParentID;
		uint32_t boneChildIDCount;

		inputArchive >> boneID >> boneName
			>> boneParentID >> boneChildIDCount;

		Init(BoneID(boneID), cd::MoveTemp(boneName));
		SetParentID(boneParentID);

		m_childIDs.resize(boneChildIDCount);
		inputArchive.ImportBuffer(GetChildIDs().data());

		inputArchive >> GetOffset() >> GetTransform();

		return *this;
	}

	template<bool SwapBytesOrder>
	const BoneImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << GetParentID().Data() << GetChildCount();
		outputArchive.ExportBuffer(GetChildIDs().data(), GetChildIDs().size());
		outputArchive << GetOffset() << GetTransform();

		return *this;
	}

private:
	BoneID m_id;
	BoneID m_parentID;
	Matrix4x4 m_offset;
	Transform m_transform;
	std::vector<BoneID> m_childIDs;
	std::string m_name;
};

}