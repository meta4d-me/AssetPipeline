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
	EXPORT_SIMPLE_TYPE_APIS(Skin, MeshID);
	EXPORT_SIMPLE_TYPE_APIS(Skin, SkeletonID);
	EXPORT_STRING_TYPE_APIS(Skin, Name);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexInfluenceBoneName);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexBoneName);
	EXPORT_VECTOR_TYPE_APIS(Skin, VertexBoneWeight);
};

}