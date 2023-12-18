#pragma once

#include "Base/Template.h"
#include "HalfEdgeMesh/HalfEdgeMesh.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/Box.hpp"
#include "Scene/Morph.h"
#include "Scene/VertexFormat.h"

#include <array>
#include <cassert>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace cd
{

class MeshImpl final
{
public:
	void FromHalfEdgeMesh(const HalfEdgeMesh& halfEdgeMesh, ConvertStrategy strategy);

public:
	DECLARE_SCENE_IMPL_CLASS(Mesh);

	void Init(uint32_t vertexCount, uint32_t polygonCount);
	
	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, ID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, MaterialID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, SkinID);
	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, VertexCount);
	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, PolygonCount);
	IMPLEMENT_COMPLEX_TYPE_APIS(Mesh, AABB);
	IMPLEMENT_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, MorphID);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexPosition);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexNormal);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexTangent);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
	IMPLEMENT_STRING_TYPE_APIS(Mesh, Name);

	void UpdateAABB();
	void ComputeVertexNormals();
	void ComputeVertexTangents();

	void SetVertexUVSetCount(uint32_t setCount);
	uint32_t GetVertexUVSetCount() const { return m_vertexUVSetCount; }
	void SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv);
	UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) { return m_vertexUVSets[setIndex][vertexIndex]; }
	const UV& GetVertexUV(uint32_t setIndex, uint32_t vertexIndex) const { return m_vertexUVSets[setIndex][vertexIndex]; }
	std::vector<UV>& GetVertexUVs(uint32_t uvSetIndex) { return m_vertexUVSets[uvSetIndex]; }
	const std::vector<UV>& GetVertexUVs(uint32_t uvSetIndex) const { return m_vertexUVSets[uvSetIndex]; }

	void SetVertexColorSetCount(uint32_t setCount);
	uint32_t GetVertexColorSetCount() const { return m_vertexColorSetCount; }
	void SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color);
	Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) { return m_vertexColorSets[setIndex][vertexIndex]; }
	const Color& GetVertexColor(uint32_t setIndex, uint32_t vertexIndex) const { return m_vertexColorSets[setIndex][vertexIndex]; }
	std::vector<Color>& GetVertexColors(uint32_t colorSetIndex) { return m_vertexColorSets[colorSetIndex]; }
	const std::vector<Color>& GetVertexColors(uint32_t colorSetIndex) const { return m_vertexColorSets[colorSetIndex]; }

	void SetVertexInfluenceCount(uint32_t influenceCount);
	uint32_t GetVertexInfluenceCount() const { return m_vertexInfluenceCount; }
	void SetVertexBoneWeight(uint32_t boneIndex, uint32_t vertexIndex, BoneID boneID, VertexWeight weight);
	BoneID GetVertexBoneID(uint32_t boneIndex, uint32_t vertexIndex) const { return m_vertexBoneIDs[boneIndex][vertexIndex]; }
	std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex) { return m_vertexBoneIDs[boneIndex]; }
	const std::vector<BoneID>& GetVertexBoneIDs(uint32_t boneIndex) const { return m_vertexBoneIDs[boneIndex]; }
	VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) { return m_vertexWeights[boneIndex][vertexIndex]; }
	const VertexWeight& GetVertexWeight(uint32_t boneIndex, uint32_t vertexIndex) const { return m_vertexWeights[boneIndex][vertexIndex]; }
	std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex) { return m_vertexWeights[boneIndex]; }
	const std::vector<VertexWeight>& GetVertexWeights(uint32_t boneIndex) const { return m_vertexWeights[boneIndex]; }

	void SetPolygon(uint32_t polygonIndex, cd::Polygon polygon);
	std::vector<Polygon>& GetPolygons() { return m_polygons; }
	const std::vector<Polygon>& GetPolygons() const { return m_polygons; }
	Polygon& GetPolygon(uint32_t polygonIndex) { return m_polygons[polygonIndex]; }
	const Polygon& GetPolygon(uint32_t polygonIndex) const { return m_polygons[polygonIndex]; }
	cd::VertexID GetPolygonVertexID(uint32_t polygonIndex, uint32_t vertexIndex) const;

	template<bool SwapBytesOrder>
	MeshImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t vertexCount;
		uint32_t vertexUVSetCount;
		uint32_t vertexColorSetCount;
		uint32_t vertexInfluenceCount;
		uint32_t polygonCount;

		inputArchive >> GetName() >> GetID().Data() >> GetMaterialID().Data() >> GetSkinID().Data()
			>> vertexCount >> vertexUVSetCount >> vertexColorSetCount >> vertexInfluenceCount
			>> polygonCount;

		Init(vertexCount, polygonCount);
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

		for (uint32_t boneIndex = 0U; boneIndex < GetVertexInfluenceCount(); ++boneIndex)
		{
			inputArchive.ImportBuffer(GetVertexBoneIDs(boneIndex).data());
			inputArchive.ImportBuffer(GetVertexWeights(boneIndex).data());
		}

		for (uint32_t polygonIndex = 0U; polygonIndex < GetPolygonCount(); ++polygonIndex)
		{
			auto& polygon = GetPolygon(polygonIndex);
			uint64_t bufferSize = inputArchive.FetchBufferSize();
			polygon.resize(bufferSize / sizeof(uint32_t));
			inputArchive.ImportBuffer(polygon.data(), bufferSize);
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const MeshImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetMaterialID().Data() << GetSkinID().Data()
			<< GetVertexCount() << GetVertexUVSetCount() << GetVertexColorSetCount() << GetVertexInfluenceCount()
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

		for (uint32_t polygonIndex = 0U; polygonIndex < GetPolygonCount(); ++polygonIndex)
		{
			outputArchive.ExportBuffer(GetPolygon(polygonIndex).data(), GetPolygon(polygonIndex).size());
		}

		return *this;
	}

private:
	uint32_t					m_vertexUVSetCount = 0U;
	uint32_t					m_vertexColorSetCount = 0U;
	uint32_t					m_vertexInfluenceCount = 0U;

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