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

	EXPORT_SIMPLE_TYPE_APIS(Mesh, ID);
	EXPORT_STRING_TYPE_APIS(Mesh, Name);
	EXPORT_COMPLEX_TYPE_APIS(Mesh, AABB);
	EXPORT_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
	EXPORT_VECTOR_TYPE_APIS(Mesh, MaterialID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, BlendShapeID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, SkinID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexInstanceToID);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexIDToInstance);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexPosition);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexNormal);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexTangent);
	EXPORT_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
	EXPORT_VECTOR_TYPE_APIS(Mesh, PolygonGroup);
	
	void Init(uint32_t vertexCount);
	void Init(uint32_t vertexCount, uint32_t vertexInstanceCount);

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
};

}