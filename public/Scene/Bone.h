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

	const BoneID& GetID() const;

	void SetName(std::string name);
	const char* GetName() const;

	void SetParentID(uint32_t parentID);
	const BoneID& GetParentID() const;

	void AddChildID(uint32_t childID);
	uint32_t GetChildCount() const;
	std::vector<BoneID>& GetChildIDs();
	const std::vector<BoneID>& GetChildIDs() const;

	Bone& operator<<(InputArchive& inputArchive);
	Bone& operator<<(InputArchiveSwapBytes& inputArchive);
	const Bone& operator>>(OutputArchive& outputArchive) const;
	const Bone& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	BoneImpl* m_pBoneImpl = nullptr;
};

}