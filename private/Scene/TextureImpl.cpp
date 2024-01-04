#include "TextureImpl.h"

namespace cd
{

TextureImpl::TextureImpl(TextureID textureID, const char* pName)
{
	Init(textureID, pName);
}

void TextureImpl::Init(TextureID textureID, std::string name)
{
	SetID(textureID);
	SetName(MoveTemp(name));

	// Assign default values
	SetUMapMode(TextureMapMode::Wrap);
	SetVMapMode(TextureMapMode::Wrap);
	SetUVOffset(cd::Vec2f::Zero());
	SetUVScale(cd::Vec2f::One());
	SetFormat(TextureFormat::Count);
	SetUseMipMap(true);
}

}