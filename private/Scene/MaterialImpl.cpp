#include "MaterialImpl.h"

#include <cassert>

namespace cd
{

MaterialImpl::MaterialImpl(MaterialID materialID, std::string materialName, MaterialType materialType)
{
	Init(materialID, MoveTemp(materialName), materialType);

	switch (materialType)
	{
		case MaterialType::BasePBR:
			InitBasePBR();
			break;

		default:
			printf("Unknow material type!\n");
			break;
	}
}

void MaterialImpl::Init(MaterialID materialID, std::string materialName, MaterialType materialType)
{
	SetID(materialID);
	SetName(cd::MoveTemp(materialName));
	SetType(materialType);
}

void MaterialImpl::InitBasePBR()
{
	SetProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::Color, Vec3f(1.0f, 1.0f, 1.0f));
	// Factor for blending Color and Texture.
	// Or if there is no Texture, then this simply scales the Color value.
	SetProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::Factor, 0.0f);
	SetProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::UseTexture, true);

	SetProperty(MaterialPropertyGroup::Occlusion, MaterialProperty::Factor, 0.0f);
	SetProperty(MaterialPropertyGroup::Occlusion, MaterialProperty::UseTexture, true);

	SetProperty(MaterialPropertyGroup::Roughness, MaterialProperty::Factor, 0.9f);
	SetProperty(MaterialPropertyGroup::Roughness, MaterialProperty::UseTexture, true);

	SetProperty(MaterialPropertyGroup::Metallic, MaterialProperty::Factor, 0.1f);
	SetProperty(MaterialPropertyGroup::Metallic, MaterialProperty::UseTexture, true);

	SetProperty(MaterialPropertyGroup::Normal, MaterialProperty::UseTexture, true);

	SetProperty(MaterialPropertyGroup::General, MaterialProperty::EnableDirectionalLights, true);
	SetProperty(MaterialPropertyGroup::General, MaterialProperty::EnablePunctualLights, true);
	SetProperty(MaterialPropertyGroup::General, MaterialProperty::EnableAreaLights, false);
	SetProperty(MaterialPropertyGroup::General, MaterialProperty::EnableIBL, true);
}

}