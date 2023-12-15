#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/Types.h"

#include <vector>

namespace cd
{

class VertexFormat;
class MorphImpl;

class CORE_API Morph final
{
public:
	static const char* GetClassName() { return "Morph"; }

public:
	Morph() = delete;
	explicit Morph(InputArchive& inputArchive);
	explicit Morph(InputArchiveSwapBytes & inputArchive);
	explicit Morph(uint32_t vertexCount);
	explicit Morph(MorphID id, MeshID sourceMeshID, const char* pName, uint32_t vertexCount);
	Morph(const Morph&) = delete;
	Morph& operator=(const Morph&) = delete;
	Morph(Morph&&);
	Morph& operator=(Morph&&);
	~Morph();

	void Init(uint32_t vertexCount);
	void Init(MorphID id, const char* pName, uint32_t vertexCount);

	EXPORT_SIMPLE_TYPE_APIS(Morph, ID);
	EXPORT_SIMPLE_TYPE_APIS(Morph, SourceMeshID);
	EXPORT_SIMPLE_TYPE_APIS(Morph, Weight);
	EXPORT_SIMPLE_TYPE_APIS(Morph, VertexCount);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexSourceID);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexPosition);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexNormal);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexTangent);
	EXPORT_VECTOR_TYPE_APIS(Morph, VertexBiTangent);
	EXPORT_STRING_TYPE_APIS(Morph, Name);

	Morph& operator<<(InputArchive& inputArchive);
	Morph& operator<<(InputArchiveSwapBytes& inputArchive);
	const Morph& operator>>(OutputArchive& outputArchive) const;
	const Morph& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MorphImpl* m_pMorphImpl = nullptr;
};

}