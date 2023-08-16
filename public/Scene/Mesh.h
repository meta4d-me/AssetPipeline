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

namespace hem
{

class HalfEdgeMesh;

}

class MeshImpl;
class VertexFormat;

class CORE_API Mesh final
{
public:
	static const char* GetClassName() { return "Mesh"; }
	static Mesh FromHalfEdgeMesh(const hem::HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy);

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

	void SetID(MeshID id);
	MeshID GetID() const;
	void SetName(const char* pName);
	const char* GetName() const;

	uint32_t GetVertexCount() const;
	uint32_t GetPolygonCount() const;

	void SetVertexFormat(VertexFormat vertexFormat);
	VertexFormat& GetVertexFormat();
	const VertexFormat& GetVertexFormat() const;

	void SetAABB(AABB aabb);
	AABB& GetAABB();
	const AABB& GetAABB() const;

	void SetMaterialID(MaterialID materialID);
	MaterialID GetMaterialID() const;

	uint32_t GetMorphCount() const;
	Morph& GetMorph(uint32_t morphIndex);
	const Morph& GetMorph(uint32_t morphIndex) const;
	std::vector<Morph>& GetMorphs();
	const std::vector<Morph>& GetMorphs() const;

	void SetVertexPosition(uint32_t vertexIndex, const Point& position);
	std::vector<Point>& GetVertexPositions();
	Point& GetVertexPosition(uint32_t vertexIndex);
	const Point& GetVertexPosition(uint32_t vertexIndex) const;
	const std::vector<Point>& GetVertexPositions() const;

	void SetVertexNormal(uint32_t vertexIndex, const Direction& normal);
	std::vector<Direction>& GetVertexNormals();
	Direction& GetVertexNormal(uint32_t vertexIndex);
	const Direction& GetVertexNormal(uint32_t vertexIndex) const;
	const std::vector<Direction>& GetVertexNormals() const;
	void ComputeVertexNormals();

	void SetVertexTangent(uint32_t vertexIndex, const Direction& tangent);
	std::vector<Direction>& GetVertexTangents();
	Direction& GetVertexTangent(uint32_t vertexIndex);
	const Direction& GetVertexTangent(uint32_t vertexIndex) const;
	const std::vector<Direction>& GetVertexTangents() const;
	void SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent);
	std::vector<Direction>& GetVertexBiTangents();
	Direction& GetVertexBiTangent(uint32_t vertexIndex);
	const Direction& GetVertexBiTangent(uint32_t vertexIndex) const;
	const std::vector<Direction>& GetVertexBiTangents() const;
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