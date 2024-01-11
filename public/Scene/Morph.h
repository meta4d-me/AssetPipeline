#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class MorphImpl;

class CORE_API Morph final
{
public:
	DECLARE_SCENE_CLASS(Morph);

	EXPORT_SIMPLE_TYPE_APIS(Morph, ID);
	EXPORT_SIMPLE_TYPE_APIS(Morph, BlendShapeID);
	EXPORT_SIMPLE_TYPE_APIS(Morph, Weight);
	EXPORT_STRING_TYPE_APIS(Morph, Name);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexSourceID);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexPosition);

	uint32_t GetVertexCount() const;
};

}