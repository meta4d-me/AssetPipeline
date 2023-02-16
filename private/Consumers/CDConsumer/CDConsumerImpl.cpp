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
	std::ofstream foutBin(m_filePath, std::ios::out | std::ios::binary);
	uint8_t target = static_cast<uint8_t>(targetEndian);
	foutBin.write(reinterpret_cast<const char*>(&target), sizeof(uint8_t));

	if (targetEndian == cd::Endian::GetNative())
	{
		cd::OutputArchive outputArchive(&foutBin);
		*pSceneDatabase >> outputArchive;
	}
	else
	{
		cd::OutputArchiveSwapBytes outputArchive(&foutBin);
		*pSceneDatabase >> outputArchive;
	}

	foutBin.close();
}

void CDConsumerImpl::ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase)
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

	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		std::string meshFileName = mesh.GetName();
		// replace "." in filename with "_" so that extension can be parsed easily.
		std::replace(meshFileName.begin(), meshFileName.end(), '.', '_');
		std::filesystem::path meshBinaryDataPath = xmlFilePath.parent_path().generic_string();
		meshBinaryDataPath += "/" + meshFileName + ".cdgeom";

		std::ofstream foutBin(meshBinaryDataPath, std::ios::out | std::ios::binary);
		cd::OutputArchive outputArchive(&foutBin);
		mesh >> outputArchive;
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
	cd::OutputArchive outputArchive(&foutBin);

	for (const auto& material : pSceneDatabase->GetMaterials())
	{
		XmlNode* pMaterialNode = WriteNode("Material");
		WriteNodeU32Attribute(pMaterialNode, "ID", material.GetID().Data());
		WriteNodeStringAttribute(pMaterialNode, "Name", material.GetName());

		XmlNode* pMaterialDataNode = WriteNode("Data");
		XmlNode *pTextureListNode = WriteNode("TextureList");

		std::vector<cd::MaterialTextureType> textureTypes = {
			cd::MaterialTextureType::BaseColor,
			cd::MaterialTextureType::Occlusion,
			cd::MaterialTextureType::Roughness,
			cd::MaterialTextureType::Metallic,
			cd::MaterialTextureType::Emissive,
			cd::MaterialTextureType::Normal,
		};

		for (const auto &textureType : textureTypes)
		{
			std::string textureKey = GetMaterialPropretyTextureKey(textureType);
			const auto &map = material.GetMaterialType();
			const std::optional<uint32_t> textureID = map.Get<uint32_t>(textureKey);

			if (textureID.has_value())
			{
				std::stringstream ss;
				ss << textureID.value();
				XmlNode* pTextureNode = WriteNode(GetMaterialPropretyGroupName(textureType), ss.str().c_str());
				pTextureListNode->append_node(pTextureNode);
			}
		}

		pMaterialNode->append_node(pMaterialDataNode);
		pMaterialNode->append_node(pTextureListNode);
		pDocument->append_node(pMaterialNode);

		material >> outputArchive;
	}

	for (const auto& texture : pSceneDatabase->GetTextures())
	{
		XmlNode* pTextureNode = WriteNode("Texture");
		WriteNodeU32Attribute(pTextureNode, "ID", texture.GetID().Data());
		WriteNodeStringAttribute(pTextureNode, "FilePath", texture.GetPath());
		pDocument->append_node(pTextureNode);

		texture >> outputArchive;
	}

	for (const auto& light : pSceneDatabase->GetLights())
	{
		XmlNode* pLightNode = WriteNode("Light");
		WriteNodeU32Attribute(pLightNode, "ID", light.GetID().Data());
		WriteNodeU32Attribute(pLightNode, "LightType", static_cast<uint32_t>(light.GetType()));
		pDocument->append_node(pLightNode);

		light >> outputArchive;
	}

	foutBin.close();

	std::ofstream foutXml(xmlFilePath, std::ios::out);
	foutXml << *pDocument;
	foutXml.close();
}

}