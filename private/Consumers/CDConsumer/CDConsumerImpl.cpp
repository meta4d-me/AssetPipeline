#include "CDConsumerImpl.h"

#include "IO/OutputArchive.hpp"
#include "Hashers/FileHash.hpp"
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

template<typename T>
void SaveInformationFile(std::string filePath, const std::filesystem::path& binaryFilePath, const T& data)
{
	// export xml readable file which contains file information and metadata.
	// XmlDocument will allocate many strings so we need to use heap memory to avoid overflow.
	std::unique_ptr<XmlDocument> pXMLDocument = CreateXmlDocumentWithDeclaration();
	XmlDocument* pDocument = pXMLDocument.get();
	XmlNode* pNode = WriteNode(pDocument, data.GetClassName());

	{
		XmlNode* pAssetNode = WriteNode(pDocument, "AssetInfo");
		WriteNodeAttribute(pDocument, pAssetNode, "ID", data.GetID().Data());
		WriteNodeAttribute(pDocument, pAssetNode, "Type", data.GetClassName());
		WriteNodeAttribute(pDocument, pAssetNode, "Name", data.GetName());
		WriteNodeAttribute(pDocument, pAssetNode, "BinaryFile", binaryFilePath.filename().string());
		WriteNodeAttribute(pDocument, pAssetNode, "BinaryHash", cd::FileHash(binaryFilePath.string().c_str()));
		pNode->append_node(pAssetNode);
	}

	auto WriteMetaDataItem = [&pDocument](XmlNode* pParentNode, std::string key, auto value)
	{
		XmlNode* pNewNode = WriteNode(pDocument, "Item");
		WriteNodeAttribute(pDocument, pNewNode, "Key", key);
		WriteNodeAttribute(pDocument, pNewNode, "Value", value);
		pParentNode->append_node(pNewNode);
	};

	XmlNode* pMetaDataNode = WriteNode(pDocument, "MetaData");
	if constexpr (std::is_same_v<cd::Mesh, T>)
	{
		WriteMetaDataItem(pMetaDataNode, "VertexCount", data.GetVertexCount());
		WriteMetaDataItem(pMetaDataNode, "TriangleCount", data.GetPolygonCount());
	}
	else if constexpr (std::is_same_v<cd::Material, T>)
	{

	}
	else if constexpr (std::is_same_v<cd::Texture, T>)
	{
		WriteMetaDataItem(pMetaDataNode, "FilePath", data.GetPath());
	}
	pNode->append_node(pMetaDataNode);

	pDocument->append_node(pNode);

	std::ofstream foutXml(filePath, std::ios::out);
	foutXml << *pDocument;
	foutXml.close();
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

void CDConsumerImpl::ExportPureBinary(const cd::SceneDatabase* pSceneDatabase)
{
	SaveBinaryFile(m_filePath, *pSceneDatabase, m_targetEndian);
}

void CDConsumerImpl::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase)
{
	std::filesystem::path exportFolderPath = m_filePath;
	exportFolderPath = exportFolderPath.parent_path();

	auto ExportSceneObject = [&exportFolderPath](const auto& object, cd::EndianType targetEndian)
	{
		std::string fileName = object.GetName();
		// replace "." in filename with "_" so that extension can be parsed easily.
		std::replace(fileName.begin(), fileName.end(), '.', '_');
		std::filesystem::path filePath = exportFolderPath / fileName;

		// export binary file.
		std::filesystem::path binaryFilePath = filePath.replace_extension(".cdbin");
		SaveBinaryFile(binaryFilePath.string(), object, targetEndian);

		std::string extensionName = ".cd";
		extensionName += object.GetClassName();
		std::transform(extensionName.begin(), extensionName.end(), extensionName.begin(), [](unsigned char c) { return std::tolower(c); });
		std::filesystem::path meshInfoFilePath = filePath.replace_extension(extensionName);
		SaveInformationFile(meshInfoFilePath.string(), binaryFilePath, object);
	};

	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		ExportSceneObject(mesh, m_targetEndian);
	}

	for (const auto& material : pSceneDatabase->GetMaterials())
	{
		ExportSceneObject(material, m_targetEndian);
	}

	for (const auto& texture : pSceneDatabase->GetTextures())
	{
		ExportSceneObject(texture, m_targetEndian);
	}
}

}