#include "CatDogConsumer.h"

#include "Scene/SceneDatabase.h"

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include <filesystem>
#include <fstream>
#include <string>

namespace cdtools
{

void CatDogConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	using XmlDocument = rapidxml::xml_document<char>;
	using XmlNode = rapidxml::xml_node<char>;
	using XmlAttribute = rapidxml::xml_attribute<char>;

	XmlDocument document;

	auto WriteNodeAttribute = [&document](XmlNode* pNode, const char* pAttributeName, const char* pAttributeValue)
	{
		pNode->append_attribute(document.allocate_attribute(document.allocate_string(pAttributeName), document.allocate_string(pAttributeValue)));
	};

	XmlNode* pDeclaration = document.allocate_node(rapidxml::node_declaration);
	WriteNodeAttribute(pDeclaration, "version", "1.0");
	WriteNodeAttribute(pDeclaration, "encoding", "utf-8");
	document.append_node(pDeclaration);

	XmlNode* pSceneNode = document.allocate_node(rapidxml::node_element, "SceneDatabase");
	WriteNodeAttribute(pSceneNode, "Name", pSceneDatabase->GetName().c_str());
	XmlNode* pSceneDataNode = document.allocate_node(rapidxml::node_element, "Data");
	WriteNodeAttribute(pSceneDataNode, "MeshCount", std::to_string(pSceneDatabase->GetMeshCount()).c_str());
	WriteNodeAttribute(pSceneDataNode, "MaterialCount", std::to_string(pSceneDatabase->GetMaterialCount()).c_str());
	WriteNodeAttribute(pSceneDataNode, "TextureCount", std::to_string(pSceneDatabase->GetTextureCount()).c_str());
	pSceneNode->append_node(pSceneDataNode);
	document.append_node(pSceneNode);

	for (const Mesh& mesh : pSceneDatabase->GetMeshes())
	{
		XmlNode* pMeshNode = document.allocate_node(rapidxml::node_element, "Mesh");
		WriteNodeAttribute(pMeshNode, "ID", std::to_string(mesh.GetID().Data()).c_str());
		WriteNodeAttribute(pMeshNode, "Name", mesh.GetName().c_str());

		XmlNode* pGeometryDataNode = document.allocate_node(rapidxml::node_element, "GeometryData");
		WriteNodeAttribute(pGeometryDataNode, "VertexCount", std::to_string(mesh.GetVertexCount()).c_str());
		WriteNodeAttribute(pGeometryDataNode, "TriangleCount", std::to_string(mesh.GetPolygonCount()).c_str());

		XmlNode* pShadingDataNode = document.allocate_node(rapidxml::node_element, "ShadingData");
		WriteNodeAttribute(pShadingDataNode, "MaterialID", std::to_string(mesh.GetMaterialID().Data()).c_str());

		pMeshNode->append_node(pGeometryDataNode);
		pMeshNode->append_node(pShadingDataNode);
		document.append_node(pMeshNode);
	}

	for (const Material& material : pSceneDatabase->GetMaterials())
	{
		XmlNode* pMaterialNode = document.allocate_node(rapidxml::node_element, "Material");
		WriteNodeAttribute(pMaterialNode, "ID", std::to_string(material.GetID().Data()).c_str());
		WriteNodeAttribute(pMaterialNode, "Name", material.GetName().c_str());

		XmlNode* pMaterialDataNode = document.allocate_node(rapidxml::node_element, "Data");
		for (const auto& [materialTextureType, textureID] : material.GetTextureIDMap())
		{
			XmlNode* pResourceNode = document.allocate_node(rapidxml::node_element, "Resource");
			WriteNodeAttribute(pResourceNode, GetMaterialTextureTypeName(materialTextureType), std::to_string(textureID.Data()).c_str());
			pMaterialDataNode->append_node(pResourceNode);
		}

		pMaterialNode->append_node(pMaterialDataNode);
		document.append_node(pMaterialNode);
	}

	for (const Texture& texture : pSceneDatabase->GetTextures())
	{
		XmlNode* pTextureNode = document.allocate_node(rapidxml::node_element, "Texture");
		WriteNodeAttribute(pTextureNode, "ID", std::to_string(texture.GetID().Data()).c_str());

		XmlNode* pTextureDataNode = document.allocate_node(rapidxml::node_element, "Data");
		WriteNodeAttribute(pTextureDataNode, "Path", texture.GetPath().c_str());

		pTextureNode->append_node(pTextureDataNode);
		document.append_node(pTextureNode);
	}

	std::filesystem::path xmlFilePath = m_filePath;
	xmlFilePath.replace_extension(".cdxml");

	std::ofstream foutXml(xmlFilePath.c_str(), std::ios::out);
	foutXml << document;
	foutXml.close();

	std::ofstream foutBin(m_filePath, std::ios::out | std::ios::binary);
	pSceneDatabase->ExportBinary(foutBin);
	foutBin.close();
}

}