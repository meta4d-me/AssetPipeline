#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/AABB.hpp"
#include "Scene/ObjectID.h"
#include "Scene/VertexFormat.h"

#include <array>
#include <string>
#include <vector>

namespace cd
{

class MeshImpl final
{
public:
	// We expect to use triangulated mesh data in game engine.
	using Polygon = TVector<VertexID, 3>;

public:
	MeshImpl() = delete;

	template<bool SwapBytesOrder>
	explicit MeshImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit MeshImpl(uint32_t vertexCount, uint32_t polygonCount);
	explicit MeshImpl(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount);

	MeshImpl(const MeshImpl&) = default;
	MeshImpl& operator=(const MeshImpl&) = default;
	MeshImpl(MeshImpl&&) = default;
	MeshImpl& operator=(MeshImpl&&) = default;
	~MeshImpl() = default;

	void Init(uint32_t vertexCount, uint32_t polygonCount);
	void Init(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount);
	const MeshID& GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }
	uint32_t GetVertexCount() const { return m_vertexCount; }
	uint32_t GetPolygonCount() const { return m_polygonCount; }

	void SetVertexFormat(VertexFormat vertexFormat) { m_vertexFormat = MoveTemp(vertexFormat); }
	VertexFormat& GetVertexFormat() { return m_vertexFormat; }
	const VertexFormat& GetVertexFormat() const { return m_vertexFormat; }

	void SetAABB(AABB aabb) { m_aabb = MoveTemp(aabb); }
	AABB& GetAABB() { return m_aabb; }
	const AABB& GetAABB() const { return m_aabb; }

	void SetMaterialID(uint32_t materialIndex) { m_materialID = materialIndex; }
	const MaterialID& GetMaterialID() const { return m_materialID; }

	void SetVertexPosition(uint32_t vertexIndex, const Point& position);
	std::vector<Point>& GetVertexPositions() { return m_vertexPositions; }
	Point& GetVertexPosition(uint32_t vertexIndex) { return m_vertexPositions[vertexIndex]; }
	const Point& GetVertexPosition(uint32_t vertexIndex) const { return m_vertexPositions[vertexIndex]; }
	const std::vector<Point>& GetVertexPositions() const { return m_vertexPositions; }

	void SetVertexNormal(uint32_t vertexIndex, const Direction& normal);
	std::vector<Direction>& GetVertexNormals() { return m_vertexNormals; }
	Direction& GetVertexNormal(uint32_t vertexIndex) { return m_vertexNormals[vertexIndex]; }
	const Direction& GetVertexNormal(uint32_t vertexIndex) const { return m_vertexNormals[vertexIndex]; }
	const std::vector<Direction>& GetVertexNormals() const { return m_vertexNormals; }

	void SetVertexTangent(uint32_t vertexIndex, const Direction& tangent);
	std::vector<Direction>& GetVertexTangents() { return m_vertexTangents; }
	Direction& GetVertexTangent(uint32_t vertexIndex) { return m_vertexTangents[vertexIndex]; }
	const Direction& GetVertexTangent(uint32_t vertexIndex) const { return m_vertexTangents[vertexIndex]; }
	const std::vector<Direction>& GetVertexTangents() const { return m_vertexTangents; }

	void SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent);
	std::vector<Direction>& GetVertexBiTangents() { return m_vertexBiTangents; }
	Direction& GetVertexBiTangent(uint32_t vertexIndex) { return m_vertexBiTangents[vertexIndex]; }
	const Direction& GetVertexBiTangent(uint32_t vertexIndex) const { return m_vertexBiTangents[vertexIndex]; }
	const std::vector<Direction>& GetVertexBiTangents() const { return m_vertexBiTangents; }

	void SetVertexUVSetCount(uint32_t setCount);
	uint32_t GetVertexUVSetCount() const { return m_vertexUVSetCount; }
	void SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv);
	std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) { return m_vertexUVSets[uvSetIndex]; }
	const std::vector<UV>& GetVertexUV(uint32_t uvSetIndex) const { return m_vertexUVSets[uvSetIndex]; }
	UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) { return m_vertexUVSets[setIndex][vertexIndex]; }
	const UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const { return m_vertexUVSets[setIndex][vertexIndex]; }

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const { return m_vertexColorSetCount; }
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) { return m_vertexColorSets[colorSetIndex]; }
	const std::vector<Color>& GetVertexColor(uint32_t colorSetIndex) const { return m_vertexColorSets[colorSetIndex]; }

	void SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2);
	std::vector<Polygon>& GetPolygons() { return m_polygons; }
	const std::vector<Polygon>& GetPolygons() const { return m_polygons; }
	const Polygon& GetPolygon(uint32_t polygonIndex) const { return m_polygons[polygonIndex]; }

	template<bool SwapBytesOrder>
	MeshImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string meshName;
		uint32_t meshID;
		uint32_t meshMaterialID;
		uint32_t vertexCount;
		uint32_t vertexUVSetCount;
		uint32_t vertexColorSetCount;
		uint32_t polygonCount;

		inputArchive >> meshName >> meshID >> meshMaterialID
			>> vertexCount >> vertexUVSetCount >> vertexColorSetCount
			>> polygonCount;

		Init(MeshID(meshID), MoveTemp(meshName), vertexCount, polygonCount);
		SetMaterialID(meshMaterialID);
		SetVertexUVSetCount(vertexUVSetCount);
		SetVertexColorSetCount(vertexColorSetCount);

		GetAABB() << inputArchive;
		GetVertexFormat() << inputArchive;
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		for (uint32_t uvSetIndex = 0; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexUV(uvSetIndex).data());
		}

		for (uint32_t colorSetIndex = 0; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexColor(colorSetIndex).data());
		}

		inputArchive.ImportBuffer(GetPolygons().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MeshImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetMaterialID().Data()
			<< GetVertexCount() << GetVertexUVSetCount() << GetVertexColorSetCount()
			<< GetPolygonCount();

		GetAABB() >> outputArchive;
		GetVertexFormat() >> outputArchive;
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());
		outputArchive.ExportBuffer(GetVertexNormals().data(), GetVertexNormals().size());
		outputArchive.ExportBuffer(GetVertexTangents().data(), GetVertexTangents().size());
		outputArchive.ExportBuffer(GetVertexBiTangents().data(), GetVertexBiTangents().size());

		for (uint32_t uvSetIndex = 0; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
		{
			outputArchive.ExportBuffer(GetVertexUV(uvSetIndex).data(), GetVertexUV(uvSetIndex).size());
		}

		for (uint32_t colorSetIndex = 0; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
		{
			outputArchive.ExportBuffer(GetVertexColor(colorSetIndex).data(), GetVertexColor(colorSetIndex).size());
		}

		outputArchive.ExportBuffer(GetPolygons().data(), GetPolygons().size());

		return *this;
	}

private:
	uint32_t				m_vertexCount = 0U;
	uint32_t				m_vertexUVSetCount = 0U;
	uint32_t				m_vertexColorSetCount = 0U;
	uint32_t				m_polygonCount = 0U;

	MeshID					m_id;
	MaterialID				m_materialID;
	std::string				m_name;
	AABB					m_aabb;
	
	// vertex geometry data
	VertexFormat			m_vertexFormat;
	std::vector<Point>		m_vertexPositions;
	std::vector<Direction>	m_vertexNormals;		// Maybe we wants to use face normals? Or we can help to calculate it.
	std::vector<Direction>	m_vertexTangents;		// Ditto.
	std::vector<Direction>	m_vertexBiTangents;		// If not stored in model file, we can help to calculate it.

	// vertex texture data
	std::vector<UV>			m_vertexUVSets[MaxUVSetNumber];
	std::vector<Color>		m_vertexColorSets[MaxColorSetNumber];

	// vertex animation data
	// vertex joint weights...

	// polygon data
	std::vector<Polygon>	m_polygons;
};

}