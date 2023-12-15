#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/TextureFormat.h"
#include "Scene/Types.h"

#include <string>
#include <vector>

namespace cd
{

class TextureImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Texture);

	explicit TextureImpl(TextureID textureID, const char* pName, MaterialTextureType textureType);
	void Init(TextureID textureID, std::string name, MaterialTextureType textureType);

	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, Type);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, Format);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, UMapMode);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, VMapMode);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, UseMipMap);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, Width);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, Height);
	IMPLEMENT_SIMPLE_TYPE_APIS(Texture, Depth);
	IMPLEMENT_COMPLEX_TYPE_APIS(Texture, UVOffset);
	IMPLEMENT_COMPLEX_TYPE_APIS(Texture, UVScale);
	IMPLEMENT_COMPLEX_TYPE_APIS(Texture, RawData);
	IMPLEMENT_STRING_TYPE_APIS(Texture, Name);
	IMPLEMENT_STRING_TYPE_APIS(Texture, Path);

	// Serialization
	template<bool SwapBytesOrder>
	TextureImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t textureID;
		std::string name;
		uint8_t textureType;
		inputArchive >> textureID >> name >> textureType;
		Init(TextureID(textureID), cd::MoveTemp(name), static_cast<cd::MaterialTextureType>(textureType));

		uint8_t textureUMapMode;
		uint8_t textureVMapMode;
		inputArchive >> textureUMapMode >> textureVMapMode >> GetUVOffset() >> GetUVScale();
		SetUMapMode(static_cast<cd::TextureMapMode>(textureUMapMode));
		SetVMapMode(static_cast<cd::TextureMapMode>(textureVMapMode));

		uint32_t textureFormat;
		bool useMipmap;
		inputArchive >> textureFormat >> useMipmap;
		SetFormat(static_cast<TextureFormat>(textureFormat));
		SetUseMipMap(useMipmap);

		size_t rawDataSize;
		inputArchive >> GetPath() >> GetWidth() >> GetHeight() >> GetDepth() >> rawDataSize;
		GetRawData().resize(rawDataSize);
		inputArchive.ImportBuffer(GetRawData().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const TextureImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetName() << static_cast<uint8_t>(GetType()) <<
			static_cast<uint8_t>(GetUMapMode()) << static_cast<uint8_t>(GetVMapMode()) << GetUVOffset() << GetUVScale() <<
			static_cast<uint32_t>(GetFormat()) << GetUseMipMap();

		outputArchive << GetPath() << GetWidth() << GetHeight() << GetDepth();
		outputArchive.ExportBuffer(GetRawData().data(), GetRawData().size());

		return *this;
	}
};

}