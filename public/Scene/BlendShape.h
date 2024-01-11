#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class BlendShapeImpl;

class CORE_API BlendShape final
{
public:
	DECLARE_SCENE_CLASS(BlendShape);

	EXPORT_SIMPLE_TYPE_APIS(BlendShape, ID);
	EXPORT_SIMPLE_TYPE_APIS(BlendShape, MeshID);
	EXPORT_STRING_TYPE_APIS(BlendShape, Name);
	EXPORT_VECTOR_TYPE_APIS(BlendShape, MorphID);
};

}