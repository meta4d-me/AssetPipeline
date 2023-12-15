#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

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

	EXPORT_SIMPLE_TYPE_APIS(Bone, ID);
	EXPORT_SIMPLE_TYPE_APIS(Bone, ParentID);
	EXPORT_COMPLEX_TYPE_APIS(Bone, Offset);
	EXPORT_COMPLEX_TYPE_APIS(Bone, Transform);
	EXPORT_VECTOR_TYPE_APIS(Bone, ChildID);
	EXPORT_STRING_TYPE_APIS(Bone, Name);

	Bone& operator<<(InputArchive& inputArchive);
	Bone& operator<<(InputArchiveSwapBytes& inputArchive);
	const Bone& operator>>(OutputArchive& outputArchive) const;
	const Bone& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	BoneImpl* m_pBoneImpl = nullptr;
};

}