#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

namespace cd
{

class SkinImpl;

class CORE_API Skin final
{
public:
	DECLARE_SCENE_CLASS(Skin);
	EXPORT_SIMPLE_TYPE_APIS(Skin, ID);
	EXPORT_SIMPLE_TYPE_APIS(Skin, SkeletonID);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexInfluenceBoneID);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexBoneIndex);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexBoneWeight);
	EXPORT_STRING_TYPE_APIS(Skin, Name);
};

}