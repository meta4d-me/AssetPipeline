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

	IMPLEMENT_SIMPLE_TYPE_APIS(Mesh, ID);
	IMPLEMENT_STRING_TYPE_APIS(Mesh, Name);
	IMPLEMENT_COMPLEX_TYPE_APIS(Mesh, AABB);
	IMPLEMENT_COMPLEX_TYPE_APIS(Mesh, VertexFormat);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, MaterialID);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, BlendShapeID);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, SkinID);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexInstanceToID);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexIDToInstance);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexPosition);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexNormal);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexTangent);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, VertexBiTangent);
	IMPLEMENT_VECTOR_TYPE_APIS(Mesh, PolygonGroup);

	void Init(uint32_t vertexCount);
	void Init(uint32_t vertexCount, uint32_t vertexInstanceCount);
	void InitVertexAttributes(uint32_t vertexInstanceCount);
	void ShrinkToFit();

	uint32_t GetVertexCount() const { return GetVertexPositionCount(); }
	uint32_t GetVertexAttributeCount() const;
	uint32_t GetPolygonCount() const;

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

	template<bool SwapBytesOrder>
	MeshImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		uint32_t materialCount;
		uint32_t blendShapeCount;
		uint32_t skinCount;
		uint32_t vertexCount;
		uint32_t vertexInstanceCount;
		uint32_t vertexUVSetCount;
		uint32_t vertexColorSetCount;
		uint32_t polygonGroupCount;
		
		inputArchive >> GetName() >> GetID().Data() >> GetAABB()
			>> materialCount >> blendShapeCount >> skinCount
			>> vertexCount >> vertexInstanceCount >> vertexUVSetCount >> vertexColorSetCount
			>> polygonGroupCount;

		GetVertexFormat() << inputArchive;

		SetMaterialIDCount(materialCount);
		inputArchive.ImportBuffer(GetMaterialIDs().data());

		SetBlendShapeIDCount(blendShapeCount);
		inputArchive.ImportBuffer(GetBlendShapeIDs().data());

		SetSkinIDCount(skinCount);
		inputArchive.ImportBuffer(GetSkinIDs().data());

		Init(vertexCount, vertexInstanceCount);
		inputArchive.ImportBuffer(GetVertexInstanceToIDs().data());
		inputArchive.ImportBuffer(GetVertexIDToInstances().data());
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		SetVertexUVSetCount(vertexUVSetCount);
		for (uint32_t uvSetIndex = 0U; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexUVs(uvSetIndex).data());
		}

		SetVertexColorSetCount(vertexColorSetCount);
		for (uint32_t colorSetIndex = 0U; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
		{
			inputArchive.ImportBuffer(GetVertexColors(colorSetIndex).data());
		}

		SetPolygonGroupCount(polygonGroupCount);
		for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < GetPolygonGroupCount(); ++polygonGroupIndex)
		{
			uint32_t polygonCount;
			inputArchive >> polygonCount;

			auto& polygonGroup = GetPolygonGroup(polygonGroupIndex);
			polygonGroup.resize(polygonCount);
			for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
			{
				auto& polygon = polygonGroup[polygonIndex];
				uint64_t bufferSize = inputArchive.FetchBufferSize();
				polygon.resize(bufferSize / sizeof(uint32_t));
				inputArchive.ImportBuffer(polygon.data(), bufferSize);
			}
		}

		return *this;
	}

	template<bool SwapBytesOrder>
	const MeshImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetAABB()
			<< GetMaterialIDCount() << GetBlendShapeIDCount() << GetSkinIDCount()
			<< GetVertexPositionCount() << GetVertexInstanceToIDCount() << GetVertexUVSetCount() << GetVertexColorSetCount()
			<< GetPolygonGroupCount();

		GetVertexFormat() >> outputArchive;
		outputArchive.ExportBuffer(GetMaterialIDs().data(), GetMaterialIDs().size());
		outputArchive.ExportBuffer(GetBlendShapeIDs().data(), GetBlendShapeIDs().size());
		outputArchive.ExportBuffer(GetSkinIDs().data(), GetSkinIDs().size());
		outputArchive.ExportBuffer(GetVertexInstanceToIDs().data(), GetVertexInstanceToIDs().size());
		outputArchive.ExportBuffer(GetVertexIDToInstances().data(), GetVertexIDToInstances().size());
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

		for (uint32_t polygonGroupIndex = 0U; polygonGroupIndex < GetPolygonGroupCount(); ++polygonGroupIndex)
		{
			const auto& polygonGroup = GetPolygonGroup(polygonGroupIndex);
			outputArchive << static_cast<uint32_t>(polygonGroup.size());

			for (uint32_t polygonIndex = 0U; polygonIndex < polygonGroup.size(); ++polygonIndex)
			{
				const auto& polygon = polygonGroup[polygonIndex];
				outputArchive.ExportBuffer(polygon.data(), polygon.size());
			}
		}

		return *this;
	}

private:
	uint32_t					m_vertexUVSetCount = 0U;
	uint32_t					m_vertexColorSetCount = 0U;

	// vertex texture data
	std::vector<UV>				m_vertexUVSets[MaxUVSetCount];
	std::vector<Color>			m_vertexColorSets[MaxColorSetCount];
};

}