#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, const char* pName, MaterialTextureType textureType)
{
	Init(textureID, pName, textureType);
}

void TextureImpl::Init(TextureID textureID, std::string name, MaterialTextureType textureType)
{
	SetID(textureID);
	SetName(MoveTemp(name));
	SetType(textureType);

	// Assign default values
	SetUMapMode(TextureMapMode::Wrap);
	SetVMapMode(TextureMapMode::Wrap);
	SetUVOffset(cd::Vec2f::Zero());
	SetUVScale(cd::Vec2f::One());
	SetFormat(TextureFormat::Count);
	SetUseMipMap(true);
}

}