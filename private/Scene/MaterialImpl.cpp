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
	m_id = materialID;
	m_name = MoveTemp(materialName);
	m_type = materialType;
}

void MaterialImpl::InitBasePBR()
{
	AddProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::Color, Vec3f(1.0f, 1.0f, 1.0f));
	// Factor for blending Color and Texture.
	// Or if there is no Texture, then this simply scales the Color value.
	AddProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::Factor, 0.0f);
	AddProperty(MaterialPropertyGroup::BaseColor, MaterialProperty::UseTexture, true);

	AddProperty(MaterialPropertyGroup::Occlusion, MaterialProperty::Factor, 0.0f);
	AddProperty(MaterialPropertyGroup::Occlusion, MaterialProperty::UseTexture, true);

	AddProperty(MaterialPropertyGroup::Roughness, MaterialProperty::Factor, 0.9f);
	AddProperty(MaterialPropertyGroup::Roughness, MaterialProperty::UseTexture, true);

	AddProperty(MaterialPropertyGroup::Metallic, MaterialProperty::Factor, 0.1f);
	AddProperty(MaterialPropertyGroup::Metallic, MaterialProperty::UseTexture, true);

	AddProperty(MaterialPropertyGroup::Normal, MaterialProperty::UseTexture, true);

	AddProperty(MaterialPropertyGroup::General, MaterialProperty::EnableDirectionalLights, true);
	AddProperty(MaterialPropertyGroup::General, MaterialProperty::EnablePunctualLights, true);
	AddProperty(MaterialPropertyGroup::General, MaterialProperty::EnableAreaLights, false);
	AddProperty(MaterialPropertyGroup::General, MaterialProperty::EnableIBL, true);
}

}