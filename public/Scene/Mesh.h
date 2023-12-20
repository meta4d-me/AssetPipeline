#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Box.hpp"
#include "Scene/Morph.h"
#include "Scene/VertexAttribute.h"

#include <vector>

namespace cd
{

class HalfEdgeMesh;
class MeshImpl;
class VertexFormat;

class CORE_API Mesh final
{
public:
	static Mesh FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy);

public:
	DECLARE_SCENE_CLASS(Mesh);
	explicit Mesh(uint32_t vertexCount);
	explicit Mesh(MeshID id, const char* pName, uint32_t vertexCount);
	void Init(uint32_t vertexCount);

	EXPORT_SIMPLE_TYPE_APIS(Mesh, ID);
	EXPORT_COMPLEX_TYPE_APIS(Mesh, AABB);
	EXPORT_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
	EXPORT_VECTOR_TYPE_APIS(Mesh, MorphID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, SkinID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexInstanceID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexPosition);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexNormal);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexTangent);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
	EXPORT_VECTOR_TYPE_APIS(Mesh, PolygonGroup);
	EXPORT_VECTOR_TYPE_APIS(Mesh, MaterialID);
	EXPORT_STRING_TYPE_APIS(Mesh, Name);

	uint32_t GetVertexCount() const;
	uint32_t GetPolygonCount() const;

	void UpdateAABB();
	void ComputeVertexNormals();
	void ComputeVertexTangents();

	void SetVertexUVSetCount(uint32_t setCount);
	uint32_t GetVertexUVSetCount() const;
	void SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv);
	std::vector<UV>& GetVertexUVs(uint32_t uvSetIndex);
	const std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) const;
	UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex);
	const UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const;

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const;
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	std::vector<Color>& GetVertexColors(uint32_t colorSetIndex);
	const std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) const;
	Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex);
	const Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) const;

	void SetVertexInfluenceCount(uint32_t influenceCount);
	uint32_t GetVertexInfluenceCount() const;
	void SetVertexBoneWeight(uint32_t boneIndex, uint32_t vertexIndex, BoneID boneID, VertexWeight weight);
	std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex);
	const std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex) const;
	BoneID GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) const;
	std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex);
	const std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex) const;
	VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex);
	const VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) const;
};

}