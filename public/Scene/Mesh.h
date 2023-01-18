#pragma once

#include "Base/Export.h"
#include "Math/AABB.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class VertexFormat;
class MeshImpl;

class CORE_API Mesh final
{
public:
	// We expect to use triangulated mesh data in game engine.
	using Polygon = TVector<VertexID, 3>;

public:
	Mesh() = delete;
	explicit Mesh(InputArchive& inputArchive);
	explicit Mesh(InputArchiveSwapBytes & inputArchive);
	explicit Mesh(uint32_t vertexCount, uint32_t polygonCount);
	explicit Mesh(MeshID meshID, const char* pMeshName, uint32_t vertexCount, uint32_t polygonCount);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&);
	Mesh& operator=(Mesh&&);
	~Mesh();

	void Init(uint32_t vertexCount, uint32_t polygonCount);
	void Init(MeshID meshID, const char* pMeshName, uint32_t vertexCount, uint32_t polygonCount);
	const MeshID& GetID() const;
	const char* GetName() const;
	uint32_t GetVertexCount() const;
	uint32_t GetPolygonCount() const;

	void SetVertexFormat(VertexFormat vertexFormat);
	VertexFormat& GetVertexFormat();
	const VertexFormat& GetVertexFormat() const;

	void SetAABB(AABB aabb);
	AABB& GetAABB();
	const AABB& GetAABB() const;

	void SetMaterialID(uint32_t materialIndex);
	const MaterialID& GetMaterialID() const;

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

	void SetVertexUVSetCount(uint32_t setCount);
	uint32_t GetVertexUVSetCount() const;
	void SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv);
	std::vector<UV>& GetVertexUV(uint32_t uvSetIndex);
	const std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) const;
	UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex);
	const UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const;

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const;
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	std::vector<Color>& GetVertexColor(uint32_t colorSetIndex);
	const std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) const;

	void SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2);
	std::vector<Polygon>& GetPolygons();
	const std::vector<Polygon>& GetPolygons() const;
	const Polygon& GetPolygon(uint32_t polygonIndex) const;

	Mesh& operator<<(InputArchive& inputArchive);
	Mesh& operator<<(InputArchiveSwapBytes& inputArchive);
	const Mesh& operator>>(OutputArchive& outputArchive) const;
	const Mesh& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MeshImpl* m_pMeshImpl = nullptr;
};

}