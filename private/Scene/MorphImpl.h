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

class MorphImpl final
{
public:
	DECLARE_SCENE_IMPL_CLASS(Morph);
	
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, BlendShapeID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, Weight);
	IMPLEMENT_STRING_TYPE_APIS(Morph, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexSourceID);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexPosition);

	uint32_t GetVertexCount() const { return GetVertexPositionCount(); }

	template<bool SwapBytesOrder>
	MorphImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t vertexPositionCount;
		inputArchive >> GetID().Data() >> GetBlendShapeID().Data() >> GetName() >> GetWeight() >> vertexPositionCount;
		SetVertexSourceIDCount(vertexPositionCount);
		SetVertexPositionCount(vertexPositionCount);
		inputArchive.ImportBuffer(GetVertexSourceIDs().data());
		inputArchive.ImportBuffer(GetVertexPositions().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MorphImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetID().Data() << GetBlendShapeID().Data() << GetName() << GetWeight() << GetVertexPositionCount();
		outputArchive.ExportBuffer(GetVertexSourceIDs().data(), GetVertexSourceIDs().size());
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());

		return *this;
	}
};

}