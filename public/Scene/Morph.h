#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

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

	EXPORT_OBJECT_ID_APIS(MorphID);
	EXPORT_NAME_APIS();

	MeshID GetSourceMeshID() const;

	void SetWeight(float weight);
	float GetWeight() const;

	uint32_t GetVertexCount() const;

	EXPORT_VECTOR_DATA_APIS(VertexSourceID);
	EXPORT_VECTOR_DATA_APIS(VertexPosition);
	EXPORT_VECTOR_DATA_APIS(VertexNormal);
	EXPORT_VECTOR_DATA_APIS(VertexTangent);
	EXPORT_VECTOR_DATA_APIS(VertexBiTangent);

	Morph& operator<<(InputArchive& inputArchive);
	Morph& operator<<(InputArchiveSwapBytes& inputArchive);
	const Morph& operator>>(OutputArchive& outputArchive) const;
	const Morph& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MorphImpl* m_pMorphImpl = nullptr;
};

}