#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class BoneImpl;

class CORE_API Bone final
{
public:
	static const char* GetClassName() { return "Bone"; }

public:
	Bone() = delete;
	explicit Bone(InputArchive& inputArchive);
	explicit Bone(InputArchiveSwapBytes& inputArchive);
	explicit Bone(BoneID id, std::string name);
	Bone(const Bone&) = delete;
	Bone& operator=(const Bone&) = delete;
	Bone(Bone&&);
	Bone& operator=(Bone&&);
	~Bone();
	
	void Init(BoneID id, std::string name);

	BoneID GetID() const;

	void SetName(std::string name);
	const char* GetName() const;

	void SetParentID(BoneID parentID);
	BoneID GetParentID() const;

	void AddChildID(BoneID childID);
	uint32_t GetChildCount() const;
	std::vector<BoneID>& GetChildIDs();
	const std::vector<BoneID>& GetChildIDs() const;

	void SetOffset(Matrix4x4 offset);
	Matrix4x4& GetOffset();
	const Matrix4x4& GetOffset() const;

	void SetTransform(Transform transform);
	Transform& GetTransform();
	const Transform& GetTransform() const;

	Bone& operator<<(InputArchive& inputArchive);
	Bone& operator<<(InputArchiveSwapBytes& inputArchive);
	const Bone& operator>>(OutputArchive& outputArchive) const;
	const Bone& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	BoneImpl* m_pBoneImpl = nullptr;
};

}