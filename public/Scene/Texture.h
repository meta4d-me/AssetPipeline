#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/TextureFormat.h"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class TextureImpl;

class CORE_API Texture final
{
public:
	DECLARE_SCENE_CLASS(Texture);
	explicit Texture(TextureID textureID, const char* pName);

	EXPORT_SIMPLE_TYPE_APIS(Texture, ID);
	EXPORT_SIMPLE_TYPE_APIS(Texture, Format);
	EXPORT_SIMPLE_TYPE_APIS(Texture, UMapMode);
	EXPORT_SIMPLE_TYPE_APIS(Texture, VMapMode);
	EXPORT_SIMPLE_TYPE_APIS(Texture, UseMipMap);
	EXPORT_SIMPLE_TYPE_APIS(Texture, Width);
	EXPORT_SIMPLE_TYPE_APIS(Texture, Height);
	EXPORT_SIMPLE_TYPE_APIS(Texture, Depth);
	EXPORT_STRING_TYPE_APIS(Texture, Name);
	EXPORT_STRING_TYPE_APIS(Texture, Path);
	EXPORT_COMPLEX_TYPE_APIS(Texture, UVOffset);
	EXPORT_COMPLEX_TYPE_APIS(Texture, UVScale);
	EXPORT_COMPLEX_TYPE_APIS(Texture, RawData);
};

}