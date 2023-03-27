#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Box.hpp"
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
	std::vector<UV>& GetVertexUVs(uint32_t uvSetIndex) { return m_vertexUVSets[uvSetIndex]; }
	const std::vector<UV>& GetVertexUVs(uint32_t uvSetIndex) const { return m_vertexUVSets[uvSetIndex]; }
	UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) { return m_vertexUVSets[setIndex][vertexIndex]; }
	const UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const { return m_vertexUVSets[setIndex][vertexIndex]; }

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const { return m_vertexColorSetCount; }
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	std::vector<Color>& GetVertexColors(uint32_t colorSetIndex) { return m_vertexColorSets[colorSetIndex]; }
	const std::vector<Color>& GetVertexColors(uint32_t colorSetIndex) const { return m_vertexColorSets[colorSetIndex]; }
	Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) { return m_vertexColorSets[setIndex][vertexIndex]; }
	const Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) const { return m_vertexColorSets[setIndex][vertexIndex]; }

	void SetVertexInfluenceCount(uint32_t influenceCount);
	uint32_t GetVertexInfluenceCount() const { return m_vertexInfluenceCount; }
	void SetVertexBoneWeight(uint32_t boneIndex, uint32_t vertexIndex, BoneID boneID, VertexWeight weight);
	std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex) { return m_vertexBoneIDs[boneIndex]; }
	const std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex) const { return m_vertexBoneIDs[boneIndex]; }
	BoneID& GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) { return m_vertexBoneIDs[boneIndex][vertexIndex]; }
	const BoneID& GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) const { return m_vertexBoneIDs[boneIndex][vertexIndex]; }
	std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex) { return m_vertexWeights[boneIndex]; }
	const std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex) const { return m_vertexWeights[boneIndex]; }
	VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) { return m_vertexWeights[boneIndex][vertexIndex]; }
	const VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) const { return m_vertexWeights[boneIndex][vertexIndex]; }

	void SetPolygon(uint32_t polygonIndex, VertexID v0, VertexID v1, VertexID v2);
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
		uint32_t vertexInfluenceCount;
		uint32_t polygonCount;

		inputArchive >> meshName >> meshID >> meshMaterialID
			>> vertexCount
			>> vertexUVSetCount >> vertexColorSetCount
			>> vertexInfluenceCount
			>> polygonCount;

		Init(MeshID(meshID), MoveTemp(meshName), vertexCount, polygonCount);
		SetMaterialID(meshMaterialID);
		SetVertexUVSetCount(vertexUVSetCount);
		SetVertexColorSetCount(vertexColorSetCount);
		SetVertexInfluenceCount(vertexInfluenceCount);

		inputArchive >> GetAABB();
		GetVertexFormat() << inputArchive;
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		for (uint32_t uvSetIndex = 0U; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexUVs(uvSetIndex).data());
		}

		for (uint32_t colorSetIndex = 0U; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexColors(colorSetIndex).data());
		}

		for(uint32_t boneIndex = 0U; boneIndex < GetVertexInfluenceCount(); ++boneIndex)
		{
			inputArchive.ImportBuffer(GetVertexBoneIDs(boneIndex).data());
			inputArchive.ImportBuffer(GetVertexWeights(boneIndex).data());
		}

		inputArchive.ImportBuffer(GetPolygons().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MeshImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetMaterialID().Data()
			<< GetVertexCount()
			<< GetVertexUVSetCount() << GetVertexColorSetCount()
			<< GetVertexInfluenceCount()
			<< GetPolygonCount();

		outputArchive << GetAABB();
		GetVertexFormat() >> outputArchive;
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());
		outputArchive.ExportBuffer(GetVertexNormals().data(), GetVertexNormals().size());
		outputArchive.ExportBuffer(GetVertexTangents().data(), GetVertexTangents().size());
		outputArchive.ExportBuffer(GetVertexBiTangents().data(), GetVertexBiTangents().size());

		for (uint32_t uvSetIndex = 0U; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
		{
			outputArchive.ExportBuffer(GetVertexUVs(uvSetIndex).data(), GetVertexUVs(uvSetIndex).size());
		}

		for (uint32_t colorSetIndex = 0U; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
		{
			outputArchive.ExportBuffer(GetVertexColors(colorSetIndex).data(), GetVertexColors(colorSetIndex).size());
		}

		for (uint32_t boneIndex = 0U; boneIndex < GetVertexInfluenceCount(); ++boneIndex)
		{
			outputArchive.ExportBuffer(GetVertexBoneIDs(boneIndex).data(), GetVertexBoneIDs(boneIndex).size());
			outputArchive.ExportBuffer(GetVertexWeights(boneIndex).data(), GetVertexWeights(boneIndex).size());
		}

		outputArchive.ExportBuffer(GetPolygons().data(), GetPolygons().size());

		return *this;
	}

private:
	uint32_t					m_vertexCount = 0U;
	uint32_t					m_vertexUVSetCount = 0U;
	uint32_t					m_vertexColorSetCount = 0U;
	uint32_t					m_vertexInfluenceCount = 0U;
	uint32_t					m_polygonCount = 0U;

	MeshID						m_id;
	MaterialID					m_materialID;
	std::string					m_name;
	AABB						m_aabb;
	
	// vertex geometry data
	VertexFormat				m_vertexFormat;
	std::vector<Point>			m_vertexPositions;
	std::vector<Direction>		m_vertexNormals;		// Maybe we wants to use face normals? Or we can help to calculate it.
	std::vector<Direction>		m_vertexTangents;		// Ditto.
	std::vector<Direction>		m_vertexBiTangents;		// If not stored in model file, we can help to calculate it.

	// vertex texture data
	std::vector<UV>				m_vertexUVSets[MaxUVSetCount];
	std::vector<Color>			m_vertexColorSets[MaxColorSetCount];

	// vertex skin data
	std::vector<BoneID>			m_vertexBoneIDs[MaxBoneInfluenceCount];
	std::vector<VertexWeight>	m_vertexWeights[MaxBoneInfluenceCount];

	// polygon data
	std::vector<Polygon>		m_polygons;
};

}