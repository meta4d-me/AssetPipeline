#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/KeyFrame.hpp"
#include "Scene/Types.h"

#include <vector>
#include <string>

namespace cd
{

class TrackImpl;

class CORE_API Track final
{
public:
	DECLARE_SCENE_CLASS(Track);

	explicit Track(TrackID id, std::string name);
	void Init(TrackID id, std::string name);

	EXPORT_SIMPLE_TYPE_APIS(Track, ID);
	EXPORT_STRING_TYPE_APIS(Track, Name);
	EXPORT_VECTOR_TYPE_APIS(Track, TranslationKey);
	EXPORT_VECTOR_TYPE_APIS(Track, RotationKey);
	EXPORT_VECTOR_TYPE_APIS(Track, ScaleKey);
};

}