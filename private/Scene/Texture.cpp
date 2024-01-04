#include "Scene/Texture.h"
#include "TextureImpl.h"

namespace cd
{

PIMPL_SCENE_CLASS(Texture);

Texture::Texture(TextureID textureID, const char* pName)
{
    m_pTextureImpl = new TextureImpl(textureID, pName);
}

PIMPL_SIMPLE_TYPE_APIS(Texture, ID);
PIMPL_SIMPLE_TYPE_APIS(Texture, Format);
PIMPL_SIMPLE_TYPE_APIS(Texture, UMapMode);
PIMPL_SIMPLE_TYPE_APIS(Texture, VMapMode);
PIMPL_SIMPLE_TYPE_APIS(Texture, UseMipMap);
PIMPL_SIMPLE_TYPE_APIS(Texture, Width);
PIMPL_SIMPLE_TYPE_APIS(Texture, Height);
PIMPL_SIMPLE_TYPE_APIS(Texture, Depth);
PIMPL_STRING_TYPE_APIS(Texture, Name);
PIMPL_STRING_TYPE_APIS(Texture, Path);
PIMPL_COMPLEX_TYPE_APIS(Texture, UVOffset);
PIMPL_COMPLEX_TYPE_APIS(Texture, UVScale);
PIMPL_COMPLEX_TYPE_APIS(Texture, RawData);

}