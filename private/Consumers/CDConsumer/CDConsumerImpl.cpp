#include "CDConsumerImpl.h"

#include "IO/OutputArchive.hpp"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/ObjectID.h"
#include "Scene/SceneDatabase.h"
#include "Scene/Texture.h"

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

namespace
{

XmlNode* WriteNode(XmlDocument* pDocument, const char* pNodeName, const char* pNodeData = nullptr)
{
	return pDocument->allocate_node(rapidxml::node_element, pDocument->allocate_string(pNodeName),
		!!pNodeData ? pDocument->allocate_string(pNodeData) : nullptr);
}

template<typename T>
void WriteNodeAttribute(XmlDocument* pDocument, XmlNode* pNode, const char* pAttributeName, const T& attributeValue)
{
	if constexpr (std::is_same_v<T, std::string>)
	{
		pNode->append_attribute(pDocument->allocate_attribute(pDocument->allocate_string(pAttributeName),
			pDocument->allocate_string(attributeValue.c_str())));
	}
	else if constexpr (std::is_arithmetic_v<T>)
	{
		pNode->append_attribute(pDocument->allocate_attribute(pDocument->allocate_string(pAttributeName),
			pDocument->allocate_string(std::to_string(attributeValue).c_str())));
	}
	else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>)
	{
		pNode->append_attribute(pDocument->allocate_attribute(pDocument->allocate_string(pAttributeName),
			pDocument->allocate_string(attributeValue)));
	}
	else
	{
		static_assert("WriteNodeAttribute : unsupported type.");
	}
}

std::unique_ptr<XmlDocument> CreateXmlDocumentWithDeclaration()
{
	std::unique_ptr<XmlDocument> pDocument = std::make_unique<XmlDocument>();
	XmlNode* pDeclaration = pDocument->allocate_node(rapidxml::node_declaration);
	WriteNodeAttribute<std::string>(pDocument.get(), pDeclaration, "version", "1.0");
	WriteNodeAttribute<std::string>(pDocument.get(), pDeclaration, "encoding", "utf-8");
	pDocument->append_node(pDeclaration);
	return pDocument;
}

template<typename T>
void SaveBinaryFile(std::string filePath, const T& data, cd::EndianType targetEndian)
{
	std::ofstream fout(filePath, std::ios::out | std::ios::binary);
	uint8_t target = static_cast<uint8_t>(targetEndian);
	fout.write(reinterpret_cast<const char*>(&target), sizeof(uint8_t));
	if (targetEndian == cd::Endian::GetNative())
	{
		cd::OutputArchive outputArchive(&fout);
		data >> outputArchive;
	}
	else
	{
		cd::OutputArchiveSwapBytes outputArchive(&fout);
		data >> outputArchive;
	}
	fout.close();
}

}

namespace cdtools
{

void CDConsumerImpl::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	switch (GetExportMode())
	{
	case ExportMode::XmlBinary:
		return ExportXmlBinary(pSceneDatabase);
	case ExportMode::PureBinary:
		return ExportPureBinary(pSceneDatabase);
	}
}

void CDConsumerImpl::ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian)
{
	SaveBinaryFile(m_filePath, *pSceneDatabase, targetEndian);
}

void CDConsumerImpl::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian)
{
	std::filesystem::path exportFolderPath = m_filePath;
	exportFolderPath = exportFolderPath.parent_path();

	std::string sceneTypeName("Scene");
	std::string materialTypeName("Material");
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		std::string meshFileName = mesh.GetName();
		// replace "." in filename with "_" so that extension can be parsed easily.
		std::replace(meshFileName.begin(), meshFileName.end(), '.', '_');
		std::filesystem::path meshFilePath = exportFolderPath / meshFileName;

		// export binary file.
		std::filesystem::path meshBinaryFilePath = meshFilePath.replace_extension(".cdbin");
		SaveBinaryFile(meshBinaryFilePath.string(), mesh, targetEndian);

		// export xml readable file which contains file information and metadata.
		// XmlDocument will allocate many strings so we need to use heap memory to avoid overflow.
		std::unique_ptr<XmlDocument> pXMLDocument = CreateXmlDocumentWithDeclaration();
		XmlDocument* pDocument = pXMLDocument.get();
		XmlNode* pMeshNode = WriteNode(pDocument, "Mesh");

		{
			XmlNode* pAssetNode = WriteNode(pDocument, "AssetInfo");
			WriteNodeAttribute(pDocument, pAssetNode, "ID", mesh.GetID().Data());
			WriteNodeAttribute(pDocument, pAssetNode, "Name", meshFileName.c_str());
			pMeshNode->append_node(pAssetNode);
		}

		{
			XmlNode* pMetaDataNode = WriteNode(pDocument, "MetaData");
			WriteNodeAttribute(pDocument, pMetaDataNode, "VertexCount", mesh.GetVertexCount());
			WriteNodeAttribute(pDocument, pMetaDataNode, "TriangleCount", mesh.GetPolygonCount());
			pMeshNode->append_node(pMetaDataNode);
		}

		{
			XmlNode* pLinksNode = WriteNode(pDocument, "Links");
			{
				XmlNode* pLinkNode = WriteNode(pDocument, "Link");
				WriteNodeAttribute(pDocument, pLinkNode, "Type", sceneTypeName);
				WriteNodeAttribute(pDocument, pLinkNode, "Name", pSceneDatabase->GetName());
				pLinksNode->append_node(pLinkNode);
			}
			{
				XmlNode* pLinkNode = WriteNode(pDocument, "Link");
				WriteNodeAttribute(pDocument, pLinkNode, "Type", materialTypeName);
				uint32_t materialID = mesh.GetMaterialID().Data();
				WriteNodeAttribute(pDocument, pLinkNode, "ID", materialID);
				WriteNodeAttribute(pDocument, pLinkNode, "Name", pSceneDatabase->GetMaterial(materialID).GetName());
				pLinksNode->append_node(pLinkNode);
			}
			pMeshNode->append_node(pLinksNode);
		}

		pDocument->append_node(pMeshNode);

		std::filesystem::path meshInfoFilePath = meshFilePath.replace_extension(".cdinfo");
		std::ofstream foutXml(meshInfoFilePath, std::ios::out);
		foutXml << *pDocument;
		foutXml.close();
	}
}

}