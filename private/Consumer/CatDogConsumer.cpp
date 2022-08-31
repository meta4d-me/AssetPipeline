#include "CatDogConsumer.h"

#include "Scene/SceneDatabase.h"

// C/C++
#include <fstream>

namespace cdtools
{

CatDogConsumer::CatDogConsumer(std::string filePath) :
	m_filePath(std::move(filePath))
{
}

void CatDogConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	std::ofstream fout(m_filePath, std::ios::out | std::ios::binary);

	size_t sceneNameLength = pSceneDatabase->GetName().size();
	fout.write(reinterpret_cast<char*>(&sceneNameLength), sizeof(sceneNameLength));
	fout.write(pSceneDatabase->GetName().c_str(), sceneNameLength);

	uint32_t meshCount = pSceneDatabase->GetMeshCount();
	uint32_t materialCount = pSceneDatabase->GetMaterialCount();
	uint32_t textureCount = pSceneDatabase->GetTextureCount();
	fout.write(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	fout.write(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
	fout.write(reinterpret_cast<char*>(&textureCount), sizeof(textureCount));

	size_t meshNameLength;
	uint32_t meshID;
	uint32_t meshMaterialID;
	uint32_t vertexCount;
	uint32_t vertexUVSetCount;
	uint32_t vertexColorSetCount;
	uint32_t polygonCount;
	size_t bufferBytes;
	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		const Mesh& mesh = pSceneDatabase->GetMesh(meshIndex);
		meshNameLength = mesh.GetName().size();
		fout.write(reinterpret_cast<char*>(&meshNameLength), sizeof(meshNameLength));
		fout.write(mesh.GetName().c_str(), meshNameLength);
		meshID = mesh.GetID().Data();
		meshMaterialID = mesh.GetMaterialID().Data();
		vertexCount = mesh.GetVertexCount();
		vertexUVSetCount = mesh.GetVertexUVSetCount();
		vertexColorSetCount = mesh.GetVertexColorSetCount();
		polygonCount = mesh.GetPolygonCount();

		fout.write(reinterpret_cast<char*>(&meshID), sizeof(meshID));
		fout.write(reinterpret_cast<char*>(&meshMaterialID), sizeof(meshMaterialID));
		fout.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
		fout.write(reinterpret_cast<char*>(&vertexUVSetCount), sizeof(vertexUVSetCount));
		fout.write(reinterpret_cast<char*>(&vertexColorSetCount), sizeof(vertexColorSetCount));
		fout.write(reinterpret_cast<char*>(&polygonCount), sizeof(polygonCount));

		bufferBytes = mesh.GetVertexPositions().size() * sizeof(Point);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(mesh.GetVertexPositions().data()), bufferBytes);

		bufferBytes = mesh.GetVertexNormals().size() * sizeof(Direction);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(mesh.GetVertexNormals().data()), bufferBytes);

		bufferBytes = mesh.GetVertexTangents().size() * sizeof(Direction);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(mesh.GetVertexTangents().data()), bufferBytes);

		bufferBytes = mesh.GetVertexBiTangents().size() * sizeof(Direction);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(mesh.GetVertexBiTangents().data()), bufferBytes);

		for (uint32_t uvSetIndex = 0; uvSetIndex < vertexUVSetCount; ++uvSetIndex)
		{
			bufferBytes = mesh.GetVertexUV(uvSetIndex).size() * sizeof(UV);
			fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
			fout.write(reinterpret_cast<const char*>(mesh.GetVertexUV(uvSetIndex).data()), bufferBytes);
		}

		for (uint32_t colorSetIndex = 0; colorSetIndex < vertexColorSetCount; ++colorSetIndex)
		{
			bufferBytes = mesh.GetVertexColor(colorSetIndex).size() * sizeof(Color);
			fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
			fout.write(reinterpret_cast<const char*>(mesh.GetVertexColor(colorSetIndex).data()), bufferBytes);
		}

		bufferBytes = mesh.GetPolygons().size() * sizeof(Mesh::Polygon);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(mesh.GetPolygons().data()), bufferBytes);
	}

	size_t texturePathLength;
	uint32_t textureID;
	for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
	{
		const Texture& texture = pSceneDatabase->GetTexture(textureIndex);
		texturePathLength = texture.GetPath().size();
		fout.write(reinterpret_cast<char*>(&texturePathLength), sizeof(texturePathLength));
		fout.write(texture.GetPath().c_str(), texturePathLength);

		textureID = texture.GetID().Data();
		fout.write(reinterpret_cast<char*>(&textureID), sizeof(textureID));
	}

	size_t materialNameLength;
	uint32_t materialID;
	size_t materialTextureCount;
	size_t textureType;
	uint32_t materialTextureID;
	for(uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
	{
		const Material& material = pSceneDatabase->GetMaterial(materialIndex);
		materialNameLength = material.GetName().size();
		fout.write(reinterpret_cast<char*>(&materialNameLength), sizeof(materialNameLength));
		fout.write(material.GetName().c_str(), materialNameLength);

		materialID = material.GetID().Data();
		fout.write(reinterpret_cast<char*>(&materialID), sizeof(materialID));

		const Material::TextureIDMap& textureIDMap = material.GetTextureIDMap();
		materialTextureCount = textureIDMap.size();
		fout.write(reinterpret_cast<char*>(&materialTextureCount), sizeof(materialTextureCount));

		for (Material::TextureIDMap::const_iterator it = textureIDMap.begin(), itEnd = textureIDMap.end();
			it != itEnd; ++it)
		{
			textureType = static_cast<size_t>(it->first);
			materialTextureID = it->second.Data();

			fout.write(reinterpret_cast<char*>(&textureType), sizeof(textureType));
			fout.write(reinterpret_cast<char*>(&materialTextureID), sizeof(materialTextureID));
		}
	}

	fout.close();
}

}