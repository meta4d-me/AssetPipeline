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
	static const char* GetClassName() { return "Mesh"; }
	static Mesh FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy);

public:
	Mesh() = default;
	explicit Mesh(InputArchive& inputArchive);
	explicit Mesh(InputArchiveSwapBytes & inputArchive);
	explicit Mesh(uint32_t vertexCount, uint32_t polygonCount);
	explicit Mesh(MeshID id, const char* pName, uint32_t vertexCount, uint32_t polygonCount);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&);
	Mesh& operator=(Mesh&&);
	~Mesh();

	void Init(uint32_t vertexCount, uint32_t polygonCount);

	EXPORT_OBJECT_ID_APIS(MeshID);
	EXPORT_NAME_APIS();
	EXPORT_VECTOR_DATA_APIS(MorphID);
	EXPORT_VECTOR_DATA_APIS(VertexPosition);
	EXPORT_VECTOR_DATA_APIS(VertexNormal);
	EXPORT_VECTOR_DATA_APIS(VertexTangent);
	EXPORT_VECTOR_DATA_APIS(VertexBiTangent);

	uint32_t GetVertexCount() const;
	uint32_t GetPolygonCount() const;

	void SetVertexFormat(VertexFormat vertexFormat);
	VertexFormat& GetVertexFormat();
	const VertexFormat& GetVertexFormat() const;

	void SetAABB(AABB aabb);
	AABB& GetAABB();
	const AABB& GetAABB() const;
	void UpdateAABB();

	void SetMaterialID(MaterialID materialID);
	MaterialID GetMaterialID() const;

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

	void SetPolygon(uint32_t polygonIndex, Polygon polygon);
	std::vector<Polygon>& GetPolygons();
	const std::vector<Polygon>& GetPolygons() const;
	Polygon& GetPolygon(uint32_t polygonIndex);
	const Polygon& GetPolygon(uint32_t polygonIndex) const;
	cd::VertexID GetPolygonVertexID(uint32_t polygonIndex, uint32_t vertexIndex) const;

	Mesh& operator<<(InputArchive& inputArchive);
	Mesh& operator<<(InputArchiveSwapBytes& inputArchive);
	const Mesh& operator>>(OutputArchive& outputArchive) const;
	const Mesh& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MeshImpl* m_pMeshImpl = nullptr;
};

}