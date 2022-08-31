#pragma once

#include "ObjectIDTypes.h"
#include "Math/Vector4.h"

#include <string>
#include <vector>

namespace cdtools
{

class Mesh final
{
public:
	// TJJ TODO : replace POD with well-designed data structure.
	struct Triangle
	{
		// Used to pass compile because we need a constructor for std::vector to use
		Triangle() {}

		//
		constexpr int GetVertexCount() const { return 3; }

		VertexID v0;
		VertexID v1;
		VertexID v2;
	};

	// We expect to use triangulated mesh data in game engine.
	using Polygon = Triangle;

public:
	Mesh() = delete;
	explicit Mesh(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount);
	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;
	~Mesh() = default;

	const MeshID& GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	uint32_t GetVertexCount() const { return m_vertexCount; }
	uint32_t GetPolygonCount() const { return m_polygonCount; }
	
	void SetMaterialID(uint32_t materialIndex);
	const MaterialID& GetMaterialID() const { return m_materialID; }

	void SetVertexPosition(uint32_t vertexIndex, const Point& position);
	std::vector<Point>& GetVertexPositions() { return m_vertexPositions; }
	const Point& GetVertexPosition(uint32_t vertexIndex) const { return m_vertexPositions[vertexIndex]; }
	const std::vector<Point>& GetVertexPositions() const { return m_vertexPositions; }

	void SetVertexNormal(uint32_t vertexIndex, const Direction& normal);
	std::vector<Direction>& GetVertexNormals() { return m_vertexNormals; }
	const Direction& GetVertexNormal(uint32_t vertexIndex) const { return m_vertexNormals[vertexIndex]; }
	const std::vector<Direction>& GetVertexNormals() const { return m_vertexNormals; }

	void SetVertexTangent(uint32_t vertexIndex, const Direction& tangent);
	std::vector<Direction>& GetVertexTangents() { return m_vertexTangents; }
	const Direction& GetVertexTangent(uint32_t vertexIndex) const { return m_vertexTangents[vertexIndex]; }
	const std::vector<Direction>& GetVertexTangents() const { return m_vertexTangents; }

	void SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent);
	std::vector<Direction>& GetVertexBiTangents() { return m_vertexBiTangents; }
	const Direction& GetVertexBiTangent(uint32_t vertexIndex) const { return m_vertexBiTangents[vertexIndex]; }
	const std::vector<Direction>& GetVertexBiTangents() const { return m_vertexBiTangents; }

	void SetVertexUVSetCount(uint32_t setCount);
	uint32_t GetVertexUVSetCount() const { return m_vertexUVSetCount; }
	void SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv);
	std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) { return m_vertexUVSets[uvSetIndex]; }
	const std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) const { return m_vertexUVSets[uvSetIndex]; }

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const { return m_vertexColorSetCount; }
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) { return m_vertexColorSets[colorSetIndex]; }
	const std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) const { return m_vertexColorSets[colorSetIndex]; }

	void SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2);
	std::vector<Polygon>& GetPolygons() { return m_polygons; }
	const std::vector<Polygon>& GetPolygons() const { return m_polygons; }
	const Polygon& GetPolygon(uint32_t polygonIndex) const { return m_polygons[polygonIndex]; }

public:
	static constexpr uint32_t MaxUVSetNumber = 8;
	static constexpr uint32_t MaxColorSetNumber = 8;

private:
	uint32_t				m_vertexCount = 0;
	uint32_t				m_vertexUVSetCount = 0;
	uint32_t				m_vertexColorSetCount = 0;
	uint32_t				m_polygonCount = 0;
	MeshID					m_id;
	MaterialID				m_materialID;

	std::string				m_name;
	
	// vertex data
	std::vector<Point>		m_vertexPositions;
	std::vector<Direction>	m_vertexNormals;		// Maybe we wants to use face normals? Or we can help to calculate it.
	std::vector<Direction>	m_vertexTangents;		// Ditto.
	std::vector<Direction>	m_vertexBiTangents;		// If not stored in model file, we can help to calculate it.
	std::vector<UV>			m_vertexUVSets[MaxUVSetNumber];
	std::vector<Color>		m_vertexColorSets[MaxColorSetNumber];

	// polygon data
	std::vector<Polygon>	m_polygons;
};

}