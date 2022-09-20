#include "Mesh.h"

#include <cassert>

namespace cdtools
{

Mesh::Mesh(std::ifstream& fin)
{
	ImportBinary(fin);
}

Mesh::Mesh(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	Init(MeshID(meshID), std::move(meshName), vertexCount, polygonCount);
}

void Mesh::Init(MeshID meshID, std::string meshName, uint32_t vertexCount, uint32_t polygonCount)
{
	m_id = meshID;
	m_name = std::move(meshName);
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
	m_polygons[polygonIndex].v0 = v0;
	m_polygons[polygonIndex].v1 = v1;
	m_polygons[polygonIndex].v2 = v2;
}

void Mesh::SetMaterialID(uint32_t materialIndex)
{
	m_materialID = materialIndex;
}

void Mesh::ImportBinary(std::ifstream& fin)
{
	std::string meshName;
	size_t meshNameLength;
	fin.read(reinterpret_cast<char*>(&meshNameLength), sizeof(meshNameLength));
	meshName.resize(meshNameLength);
	fin.read(const_cast<char*>(meshName.data()), meshNameLength);

	uint32_t meshID;
	fin.read(reinterpret_cast<char*>(&meshID), sizeof(meshID));

	uint32_t meshMaterialID;
	fin.read(reinterpret_cast<char*>(&meshMaterialID), sizeof(meshMaterialID));

	uint32_t vertexCount;
	fin.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
	
	uint32_t vertexUVSetCount;
	fin.read(reinterpret_cast<char*>(&vertexUVSetCount), sizeof(vertexUVSetCount));

	uint32_t vertexColorSetCount;
	fin.read(reinterpret_cast<char*>(&vertexColorSetCount), sizeof(vertexColorSetCount));

	uint32_t polygonCount;
	fin.read(reinterpret_cast<char*>(&polygonCount), sizeof(polygonCount));

	Init(MeshID(meshID), std::move(meshName), vertexCount, polygonCount);
	SetMaterialID(meshMaterialID);
	SetVertexUVSetCount(vertexUVSetCount);
	SetVertexColorSetCount(vertexColorSetCount);

	size_t bufferBytes;
	fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
	fin.read(reinterpret_cast<char*>(GetVertexPositions().data()), bufferBytes);

	fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
	fin.read(reinterpret_cast<char*>(GetVertexNormals().data()), bufferBytes);

	fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
	fin.read(reinterpret_cast<char*>(GetVertexTangents().data()), bufferBytes);

	fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
	fin.read(reinterpret_cast<char*>(GetVertexBiTangents().data()), bufferBytes);

	for (uint32_t uvSetIndex = 0; uvSetIndex < GetVertexUVSetCount(); ++uvSetIndex)
	{
		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(GetVertexUV(uvSetIndex).data()), bufferBytes);
	}

	for (uint32_t colorSetIndex = 0; colorSetIndex < GetVertexColorSetCount(); ++colorSetIndex)
	{
		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(GetVertexColor(colorSetIndex).data()), bufferBytes);
	}

	fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
	fin.read(reinterpret_cast<char*>(GetPolygons().data()), bufferBytes);
}

void Mesh::ExportBinary(std::ofstream& fout) const
{
	size_t meshNameLength = GetName().size();
	fout.write(reinterpret_cast<char*>(&meshNameLength), sizeof(meshNameLength));
	fout.write(GetName().c_str(), meshNameLength);
	uint32_t meshID = GetID().Data();
	uint32_t meshMaterialID = GetMaterialID().Data();
	uint32_t vertexCount = GetVertexCount();
	uint32_t vertexUVSetCount = GetVertexUVSetCount();
	uint32_t vertexColorSetCount = GetVertexColorSetCount();
	uint32_t polygonCount = GetPolygonCount();

	fout.write(reinterpret_cast<char*>(&meshID), sizeof(meshID));
	fout.write(reinterpret_cast<char*>(&meshMaterialID), sizeof(meshMaterialID));
	fout.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
	fout.write(reinterpret_cast<char*>(&vertexUVSetCount), sizeof(vertexUVSetCount));
	fout.write(reinterpret_cast<char*>(&vertexColorSetCount), sizeof(vertexColorSetCount));
	fout.write(reinterpret_cast<char*>(&polygonCount), sizeof(polygonCount));

	size_t bufferBytes = GetVertexPositions().size() * sizeof(Point);
	fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
	fout.write(reinterpret_cast<const char*>(GetVertexPositions().data()), bufferBytes);

	bufferBytes = GetVertexNormals().size() * sizeof(Direction);
	fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
	fout.write(reinterpret_cast<const char*>(GetVertexNormals().data()), bufferBytes);

	bufferBytes = GetVertexTangents().size() * sizeof(Direction);
	fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
	fout.write(reinterpret_cast<const char*>(GetVertexTangents().data()), bufferBytes);

	bufferBytes = GetVertexBiTangents().size() * sizeof(Direction);
	fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
	fout.write(reinterpret_cast<const char*>(GetVertexBiTangents().data()), bufferBytes);

	for (uint32_t uvSetIndex = 0; uvSetIndex < vertexUVSetCount; ++uvSetIndex)
	{
		bufferBytes = GetVertexUV(uvSetIndex).size() * sizeof(UV);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(GetVertexUV(uvSetIndex).data()), bufferBytes);
	}

	for (uint32_t colorSetIndex = 0; colorSetIndex < vertexColorSetCount; ++colorSetIndex)
	{
		bufferBytes = GetVertexColor(colorSetIndex).size() * sizeof(Color);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(GetVertexColor(colorSetIndex).data()), bufferBytes);
	}

	bufferBytes = GetPolygons().size() * sizeof(Mesh::Polygon);
	fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
	fout.write(reinterpret_cast<const char*>(GetPolygons().data()), bufferBytes);
}

}