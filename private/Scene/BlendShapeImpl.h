#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <array>
#include <string>
#include <vector>

namespace cd
{

class BlendShapeImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(BlendShape);
	
	IMPLEMENT_SIMPLE_TYPE_APIS(BlendShape, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(BlendShape, MeshID);
	IMPLEMENT_STRING_TYPE_APIS(BlendShape, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(BlendShape, MorphID);

	template<bool SwapBytesOrder>
	BlendShapeImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t morphCount;
		inputArchive >> GetID().Data() >> GetMeshID().Data() >> GetName() >> morphCount;
		SetMorphIDCount(morphCount);
		inputArchive.ImportBuffer(GetMorphIDs().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const BlendShapeImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetMeshID().Data() << GetName() << GetMorphIDCount();
		outputArchive.ExportBuffer(GetMorphIDs().data(), GetMorphIDs().size());

		return *this;
	}
};

}