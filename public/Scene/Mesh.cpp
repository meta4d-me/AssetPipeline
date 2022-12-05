#include "Mesh.h"

#include <cassert>

namespace cd
{

Mesh::Mesh(std::ifstream& fin)
{
	ImportBinary(fin);
}

Mesh::Mesh(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	Init(MeshID(meshID), cd::MoveTemp(meshName), vertexCount, polygonCount);
}

void Mesh::Init(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_id = meshID;
	m_name = cd::MoveTemp(meshName);
	m_vertexCount = vertexCount;
	m_polygonCount = polygonCount;

	assert(vertexCount > 0 && "No need to create an empty mesh.");
	assert(polygonCount > 0 && "Expect to generate index buffer by ourselves?");

	// pre-construct for attributes which almost all model files will have.
	m_vertexPositions.resize(vertexCount);
	m_vertexNormals.resize(vertexCount);
	m_vertexTangents.resize(vertexCount);
	m_vertexBiTangents.resize(vertexCount);

	m_polygons.resize(polygonCount);
}

void Mesh::SetVertexPosition(uint32_t vertexIndex, const Point& position)
{
	m_vertexPositions[vertexIndex] = position;
}

void Mesh::SetVertexNormal(uint32_t vertexIndex, const Direction& normal)
{
	m_vertexNormals[vertexIndex] = normal;
}

void Mesh::SetVertexTangent(uint32_t vertexIndex, const Direction& tangent)
{
	m_vertexTangents[vertexIndex] = tangent;
}

void Mesh::SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent)
{
	m_vertexBiTangents[vertexIndex] = biTangent;
}

void Mesh::SetVertexUVSetCount(uint32_t setCount)
{
	m_vertexUVSetCount = setCount;
	for(uint32_t i = 0; i < m_vertexUVSetCount; ++i)
	{
		m_vertexUVSets[i].resize(m_vertexCount);
	}
}

void Mesh::SetVertexUV(uint32_t setIndex, uint32_t vertexIndex, const UV& uv)
{
	m_vertexUVSets[setIndex][vertexIndex] = uv;
}

void Mesh::SetVertexColorSetCount(uint32_t setCount)
{
	m_vertexColorSetCount = setCount;
	for (uint32_t i = 0; i < m_vertexColorSetCount; ++i)
	{
		m_vertexColorSets[i].resize(m_vertexCount);
	}
}

void Mesh::SetVertexColor(uint32_t setIndex, uint32_t vertexIndex, const Color& color)
{
	m_vertexColorSets[setIndex][vertexIndex] = color;
}

void Mesh::SetPolygon(uint32_t polygonIndex, const VertexID& v0, const VertexID& v1, const VertexID& v2)
{
	m_polygons[polygonIndex][0] = v0;
	m_polygons[polygonIndex][1] = v1;
	m_polygons[polygonIndex][2] = v2;
}

void Mesh::ImportBinary(std::ifstream& fin)
{
	std::string meshName;
	ImportData(fin, meshName);

	uint32_t meshID;
	uint32_t meshMaterialID;
	uint32_t vertexCount;
	uint32_t vertexUVSetCount;
	uint32_t vertexColorSetCount;
	uint32_t polygonCount;
	ImportData(fin, meshID);
	ImportData(fin, meshMaterialID);
	ImportData(fin, vertexCount);
	ImportData(fin, vertexUVSetCount);
	ImportData(fin, vertexColorSetCount);
	ImportData(fin, polygonCount);

	Init(MeshID(meshID), cd::MoveTemp(meshName), vertexCount, polygonCount);
	SetMaterialID(meshMaterialID);
	SetVertexUVSetCount(vertexUVSetCount);
	SetVertexColorSetCount(vertexColorSetCount);

	GetAABB().ImportBinary(fin);
	GetVertexFormat().ImportBinary(fin);
	ImportDataBuffer(fin, GetVertexPositions().data());
	ImportDataBuffer(fin, GetVertexNormals().data());
	ImportDataBuffer(fin, GetVertexTangents().data());
	ImportDataBuffer(fin, GetVertexBiTangents().data());

	for (uint32_t uvSetIndex = 0; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
	{
		ImportDataBuffer(fin, GetVertexUV(uvSetIndex).data());
	}

	for (uint32_t colorSetIndex = 0; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
	{
		ImportDataBuffer(fin, GetVertexColor(colorSetIndex).data());
	}

	ImportDataBuffer(fin, GetPolygons().data());
}

void Mesh::ExportBinary(std::ofstream& fout) const
{
	ExportData<std::string>(fout, GetName());

	ExportData<uint32_t>(fout, GetID().Data());
	ExportData<uint32_t>(fout, GetMaterialID().Data());
	ExportData<uint32_t>(fout, GetVertexCount());
	ExportData<uint32_t>(fout, GetVertexUVSetCount());
	ExportData<uint32_t>(fout, GetVertexColorSetCount());
	ExportData<uint32_t>(fout, GetPolygonCount());

	GetAABB().ExportBinary(fout);
	GetVertexFormat().ExportBinary(fout);
	ExportDataBuffer(fout, GetVertexPositions().data(), GetVertexPositions().size());
	ExportDataBuffer(fout, GetVertexNormals().data(), GetVertexNormals().size());
	ExportDataBuffer(fout, GetVertexTangents().data(), GetVertexTangents().size());
	ExportDataBuffer(fout, GetVertexBiTangents().data(), GetVertexBiTangents().size());

	for (uint32_t uvSetIndex = 0; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
	{
		ExportDataBuffer(fout, GetVertexUV(uvSetIndex).data(), GetVertexUV(uvSetIndex).size());
	}

	for (uint32_t colorSetIndex = 0; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
	{
		ExportDataBuffer(fout, GetVertexColor(colorSetIndex).data(), GetVertexColor(colorSetIndex).size());
	}

	ExportDataBuffer(fout, GetPolygons().data(), GetPolygons().size());
}

}