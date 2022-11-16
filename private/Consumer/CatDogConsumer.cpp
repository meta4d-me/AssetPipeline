#include "CatDogConsumer.h"

#include "Scene/SceneDatabase.h"

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
using XmlDocument = rapidxml::xml_document<char>;
using XmlNode = rapidxml::xml_node<char>;
using XmlAttribute = rapidxml::xml_attribute<char>;

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace cdtools
{

void CatDogConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	switch (GetExportMode())
	{
	case ExportMode::XmlBinary:
		return ExportXmlBinary(pSceneDatabase);
	case ExportMode::PureBinary:
		return ExportPureBinary(pSceneDatabase);
	}
}

void CatDogConsumer::ExportPureBinary(const SceneDatabase* pSceneDatabase)
{
	std::ofstream foutBin(m_filePath, std::ios::out | std::ios::binary);
	pSceneDatabase->ExportBinary(foutBin);
	foutBin.close();
}

void CatDogConsumer::ExportXmlBinary(const SceneDatabase* pSceneDatabase)
{
	std::filesystem::path xmlFilePath = m_filePath;
	xmlFilePath.replace_extension(".cdxml");

	// XmlDocument will allocate many strings so we need to use heap memory to avoid overflow.
	std::unique_ptr<XmlDocument> pDocument = std::make_unique<XmlDocument>();

	auto WriteNode = [&pDocument](const char* pNodeName, const char* pNodeData = nullptr)
	{
		return pDocument->allocate_node(rapidxml::node_element, pDocument->allocate_string(pNodeName),
			!!pNodeData ? pDocument->allocate_string(pNodeData) : nullptr);
	};
	
	auto WriteNodeU32Attribute = [&pDocument](XmlNode* pNode, const char* pAttributeName, uint32_t attributeValue)
	{
		pNode->append_attribute(pDocument->allocate_attribute(pDocument->allocate_string(pAttributeName),
			pDocument->allocate_string(std::to_string(attributeValue).c_str())));
	};

	auto WriteNodeStringAttribute = [&pDocument](XmlNode* pNode, const char* pAttributeName, const std::string& attributeValue)
	{
		pNode->append_attribute(pDocument->allocate_attribute(pDocument->allocate_string(pAttributeName),
			pDocument->allocate_string(attributeValue.c_str())));
	};

	XmlNode* pDeclaration = pDocument->allocate_node(rapidxml::node_declaration);
	WriteNodeStringAttribute(pDeclaration, "version", "1.0");
	WriteNodeStringAttribute(pDeclaration, "encoding", "utf-8");
	pDocument->append_node(pDeclaration);

	XmlNode* pSceneNode = WriteNode("SceneDatabase");
	WriteNodeStringAttribute(pSceneNode, "Name", pSceneDatabase->GetName());
	XmlNode* pSceneDataNode = WriteNode("Data");
	WriteNodeU32Attribute(pSceneDataNode, "MeshCount", pSceneDatabase->GetMeshCount());
	WriteNodeU32Attribute(pSceneDataNode, "MaterialCount", pSceneDatabase->GetMaterialCount());
	WriteNodeU32Attribute(pSceneDataNode, "TextureCount", pSceneDatabase->GetTextureCount());
	pSceneNode->append_node(pSceneDataNode);
	pDocument->append_node(pSceneNode);

	for (const Mesh& mesh : pSceneDatabase->GetMeshes())
	{
		std::string meshFileName = mesh.GetName();
		// replace "." in filename with "_" so that extension can be parsed easily.
		std::replace(meshFileName.begin(), meshFileName.end(), '.', '_');
		std::filesystem::path meshBinaryDataPath = std::format("{}/{}", xmlFilePath.parent_path().generic_string(), meshFileName + ".cdgeom");
		std::ofstream foutBin(meshBinaryDataPath, std::ios::out | std::ios::binary);
		mesh.ExportBinary(foutBin);
		foutBin.close();

		XmlNode* pMeshNode = WriteNode("Mesh");
		WriteNodeU32Attribute(pMeshNode, "ID", mesh.GetID().Data());
		WriteNodeStringAttribute(pMeshNode, "Name", mesh.GetName());

		XmlNode* pFilePathNode = WriteNode("FilePath", meshBinaryDataPath.generic_string().c_str());

		XmlNode* pGeometryNode = WriteNode("Geometry");
		XmlNode* pGeometryDataNode = WriteNode("Data");
		WriteNodeU32Attribute(pGeometryDataNode, "VertexCount", mesh.GetVertexCount());
		WriteNodeU32Attribute(pGeometryDataNode, "TriangleCount", mesh.GetPolygonCount());
		pGeometryNode->append_node(pGeometryDataNode);

		XmlNode* pShadingNode = WriteNode("Shading");
		XmlNode* pShadingDataNode = WriteNode("Data");
		WriteNodeU32Attribute(pShadingDataNode, "MaterialID", mesh.GetMaterialID().Data());
		pShadingNode->append_node(pShadingDataNode);

		pMeshNode->append_node(pFilePathNode);
		pMeshNode->append_node(pGeometryNode);
		pMeshNode->append_node(pShadingNode);
		pDocument->append_node(pMeshNode);
	}

	// Write remain materials + textures to binary file.
	// TODO : we should add texture compiling process to AssetPipeline.
	std::ofstream foutBin(m_filePath, std::ios::out | std::ios::binary);

	for (const Material& material : pSceneDatabase->GetMaterials())
	{
		XmlNode* pMaterialNode = WriteNode("Material");
		WriteNodeU32Attribute(pMaterialNode, "ID", material.GetID().Data());
		WriteNodeStringAttribute(pMaterialNode, "Name", material.GetName());

		XmlNode* pMaterialDataNode = WriteNode("Data");
		WriteNodeU32Attribute(pMaterialDataNode, "TextureCount", static_cast<uint32_t>(material.GetTextureIDMap().size()));

		XmlNode* pTextureListNode = WriteNode("TextureList");
		for (const auto& [materialTextureType, textureID] : material.GetTextureIDMap())
		{
			XmlNode* pTextureNode = WriteNode(GetMaterialTextureTypeName(materialTextureType), std::to_string(textureID.Data()).c_str());
			pTextureListNode->append_node(pTextureNode);
		}

		pMaterialNode->append_node(pMaterialDataNode);
		pMaterialNode->append_node(pTextureListNode);
		pDocument->append_node(pMaterialNode);

		material.ExportBinary(foutBin);
	}

	for (const Texture& texture : pSceneDatabase->GetTextures())
	{
		XmlNode* pTextureNode = WriteNode("Texture");
		WriteNodeU32Attribute(pTextureNode, "ID", texture.GetID().Data());
		WriteNodeStringAttribute(pTextureNode, "FilePath", texture.GetPath());
		pDocument->append_node(pTextureNode);

		texture.ExportBinary(foutBin);
	}

	foutBin.close();

	std::ofstream foutXml(xmlFilePath, std::ios::out);
	foutXml << *pDocument;
	foutXml.close();
}

}