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

	explicit MorphImpl(uint32_t vertexCount);
	explicit MorphImpl(MorphID id, MeshID sourceMeshID, std::string name, uint32_t vertexCount);
	void Init(uint32_t vertexCount);
	void Init(MorphID id, std::string name, uint32_t vertexCount);
	
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, SourceMeshID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, Weight);
	IMPLEMENT_SIMPLE_TYPE_APIS(Morph, VertexCount);
	IMPLEMENT_STRING_TYPE_APIS(Morph, Name);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexSourceID);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexPosition);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexNormal);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexTangent);
	IMPLEMENT_VECTOR_TYPE_APIS(Morph, VertexBiTangent);

	template<bool SwapBytesOrder>
	MorphImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string name;
		uint32_t id;
		uint32_t vertexCount;
		inputArchive >> name >> id >> vertexCount;

		Init(MorphID(id), MoveTemp(name), vertexCount);
		inputArchive >> GetWeight();
		inputArchive.ImportBuffer(GetVertexSourceIDs().data());
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MorphImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetVertexCount() << GetWeight();
		outputArchive.ExportBuffer(GetVertexSourceIDs().data(), GetVertexSourceIDs().size());
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());
		outputArchive.ExportBuffer(GetVertexNormals().data(), GetVertexNormals().size());
		outputArchive.ExportBuffer(GetVertexTangents().data(), GetVertexTangents().size());
		outputArchive.ExportBuffer(GetVertexBiTangents().data(), GetVertexBiTangents().size());

		return *this;
	}
};

}