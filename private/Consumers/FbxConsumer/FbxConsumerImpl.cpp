#include "FbxConsumerImpl.h"

#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

#include <cassert>
#include <format>

namespace
{

void PrintLog(const std::string& msg)
{
	printf("[AssetPipeline][FbxConsumer] %s\n", msg.c_str());
}

}

namespace cdtools
{

FbxConsumerImpl::FbxConsumerImpl(std::string filePath) :
	m_filePath(cd::MoveTemp(filePath))
{
	// Default
	m_options.Enable(FbxConsumerOptions::SaveAsASCII);
	m_options.Enable(FbxConsumerOptions::ExportMaterial);
	m_options.Enable(FbxConsumerOptions::ExportTexture);
	m_options.Enable(FbxConsumerOptions::ExportStaticMesh);
}

FbxConsumerImpl::~FbxConsumerImpl()
{
	if(m_pSDKManager)
	{
		m_pSDKManager->Destroy();
	}
}

void FbxConsumerImpl::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	// Init settings.
	m_pSDKManager = fbxsdk::FbxManager::Create();
	auto* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, IOSROOT);
	m_pSDKManager->SetIOSettings(pIOSettings);

	fbxsdk::FbxScene* pScene = CreateScene(pSceneDatabase);
	assert(pScene);

	// Init node hierarchy.
	for (auto rootNodeID : pSceneDatabase->GetRootNodeIDs())
	{
		ExportNodeRecursively(pScene, pScene->GetRootNode(), rootNodeID, pSceneDatabase);
	}

	// Query mesh's associated fbx node.
	std::map<cd::MeshID, fbxsdk::FbxNode*> mapMeshIDToFbxNode;
	for (const cd::Node& node : pSceneDatabase->GetNodes())
	{
		for (cd::MeshID meshID : node.GetMeshIDs())
		{
			auto itMapping = mapMeshIDToFbxNode.find(meshID);
			if (itMapping == mapMeshIDToFbxNode.end())
			{
				fbxsdk::FbxNode* pFbxNode = pScene->FindNodeByName(node.GetName());
				assert(pFbxNode);
				mapMeshIDToFbxNode[meshID] = pFbxNode;
			}
			else
			{
				PrintLog("Error : Two node contains same mesh id?");
			}
		}
	}

	// Export meshes to according nodes.
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		auto itMapping = mapMeshIDToFbxNode.find(mesh.GetID());
		fbxsdk::FbxNode* pNode = itMapping == mapMeshIDToFbxNode.end() ? nullptr : itMapping->second;
		ExportMesh(pScene, pNode, mesh.GetID(), pSceneDatabase);
	}

	if (!ExportFbxFile(pScene))
	{
		PrintLog("Error : Failed to export fbx file.");
	}
}

fbxsdk::FbxScene* FbxConsumerImpl::CreateScene(const cd::SceneDatabase* pSceneDatabase)
{
	auto* pScene = fbxsdk::FbxScene::Create(m_pSDKManager, pSceneDatabase->GetName());

	// Tag source information.
	auto* pDocument = fbxsdk::FbxDocumentInfo::Create(m_pSDKManager, "Document");
	pDocument->mTitle = "FBX Exporter";
	pDocument->mSubject = "Export FBX";
	pScene->SetSceneInfo(pDocument);

	// Set AxisSystem and Unit.
	auto frontVector = fbxsdk::FbxAxisSystem::eParityOdd;
	fbxsdk::FbxAxisSystem axisSystem(fbxsdk::FbxAxisSystem::eYAxis, frontVector, fbxsdk::FbxAxisSystem::eLeftHanded);
	auto& globalSettings = pScene->GetGlobalSettings();
	globalSettings.SetAxisSystem(axisSystem);
	globalSettings.SetOriginalUpAxis(axisSystem);
	globalSettings.SetSystemUnit(fbxsdk::FbxSystemUnit::cm);
	globalSettings.SetTimeMode(fbxsdk::FbxTime::eDefaultMode);

	return pScene;
}

fbxsdk::FbxFileTexture* FbxConsumerImpl::ExportTexture(fbxsdk::FbxScene* pScene, cd::TextureID textureID, const cd::SceneDatabase* pSceneDatabase)
{
	assert(pScene);
	if (!textureID.IsValid() || textureID.Data() >= pSceneDatabase->GetTextureCount())
	{
		PrintLog(std::format("Warning : [Texture {}] is invalid or lost reference data.", textureID.Data()));
		return nullptr;
	}

	const cd::Texture& texture = pSceneDatabase->GetTexture(textureID.Data());
	const char* pTextureName = texture.GetName();
	auto* pTexture = static_cast<fbxsdk::FbxFileTexture*>(pScene->GetTexture(pTextureName));
	if (pTexture)
	{
		return pTexture;
	}

	// TODO : support other texture types.
	pTexture = fbxsdk::FbxFileTexture::Create(pScene, pTextureName);
	pTexture->SetFileName(texture.GetPath());
	pTexture->SetTextureUse(FbxTexture::eStandard);
	pTexture->SetMappingType(FbxTexture::eUV);
	return pTexture;
}

fbxsdk::FbxSurfaceMaterial* FbxConsumerImpl::ExportMaterial(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode, cd::MaterialID materialID, const cd::SceneDatabase* pSceneDatabase)
{
	assert(pScene && pNode);
	bool useDefaultMaterial = !materialID.IsValid() || materialID.Data() >= pSceneDatabase->GetMaterialCount();
	if (useDefaultMaterial)
	{
		constexpr const char* DefaultMaterialName = "Default";
		fbxsdk::FbxSurfaceMaterial* pFbxMaterial = pScene->GetMaterial(DefaultMaterialName);
		if (!pFbxMaterial)
		{
			auto* pLambert = fbxsdk::FbxSurfaceLambert::Create(pScene, DefaultMaterialName);
			pLambert->Diffuse.Set(fbxsdk::FbxDouble3(0.72));
			pFbxMaterial = pLambert;
		}

		return pFbxMaterial;
	}

	const cd::Material& material = pSceneDatabase->GetMaterial(materialID.Data());
	const char* pMaterialName = material.GetName();
	fbxsdk::FbxSurfaceMaterial* pFbxMaterial = pScene->GetMaterial(pMaterialName);
	if (pFbxMaterial)
	{
		return pFbxMaterial;
	}

	pFbxMaterial = fbxsdk::FbxSurfaceLambert::Create(m_pSDKManager, pMaterialName);
	
	// Fill parameter info.
	// TODO.

	// Fill texture info.
	static std::map<cd::MaterialTextureType, const char*> MapTextureTypeToFbxPropertyName;
	MapTextureTypeToFbxPropertyName[cd::MaterialTextureType::BaseColor] = fbxsdk::FbxSurfaceMaterial::sDiffuse;
	MapTextureTypeToFbxPropertyName[cd::MaterialTextureType::Normal] = fbxsdk::FbxSurfaceMaterial::sNormalMap;
	MapTextureTypeToFbxPropertyName[cd::MaterialTextureType::Emissive] = fbxsdk::FbxSurfaceMaterial::sEmissive;
	for (int textureTypeValue = 0; textureTypeValue < nameof::enum_count<cd::MaterialTextureType>(); ++textureTypeValue)
	{
		auto textureType = static_cast<cd::MaterialTextureType>(textureTypeValue);
		auto itProperty = MapTextureTypeToFbxPropertyName.find(textureType);
		if (itProperty == MapTextureTypeToFbxPropertyName.end())
		{
			continue;
		}

		fbxsdk::FbxProperty textureProperty = pFbxMaterial->FindProperty(itProperty->second);
		assert(textureProperty.IsValid()); // Wrong mapping?
		if (material.IsTextureSetup(textureType))
		{
			fbxsdk::FbxFileTexture* pFileTexture = ExportTexture(pScene, material.GetTextureID(textureType), pSceneDatabase);
			if (pFileTexture)
			{
				pFileTexture->ConnectDstProperty(textureProperty);
			}
		}
	}

	return pFbxMaterial;
}

void FbxConsumerImpl::ExportMesh(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode, cd::MeshID meshID, const cd::SceneDatabase* pSceneDatabase)
{
	assert(pScene);
	if (!meshID.IsValid() || meshID.Data() >= pSceneDatabase->GetMeshCount())
	{
		PrintLog(std::format("Warning : [Mesh {}] is invalid or lost reference data.", meshID.Data()));
		return;
	}

	const auto& mesh = pSceneDatabase->GetMesh(meshID.Data());

	if (!pNode)
	{
		// FbxMesh is a derived class of FbxNodeAttribute which always need to add to FbxNode.
		pNode = ExportNode(pScene, mesh.GetName(), cd::Transform::Identity(), pSceneDatabase);
	}

	uint32_t vertexCount = mesh.GetVertexCount();
	uint32_t polygonCount = mesh.GetPolygonCount();
	assert(vertexCount > 0U && polygonCount > 0U);

	auto* pFbxMesh = fbxsdk::FbxMesh::Create(pScene, mesh.GetName());
	pNode->SetNodeAttribute(pFbxMesh);

	// Export position.
	pFbxMesh->InitControlPoints(vertexCount);
	fbxsdk::FbxVector4* pFbxVertices = pFbxMesh->GetControlPoints();
	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
		pFbxVertices[vertexIndex].Set(position.x(), -position.y(), position.z(), 1.0f);
	}

	// Create mesh base layer.
	int baseLayerIndex = pFbxMesh->CreateLayer();
	assert(0 == baseLayerIndex);
	fbxsdk::FbxLayer* pBaseLayer = pFbxMesh->GetLayer(baseLayerIndex);

	// Create base normal for every vertex which directly maps to control point.
	auto* pNormalElement = fbxsdk::FbxLayerElementNormal::Create(pFbxMesh, "BaseNormal");
	pNormalElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
	pNormalElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);
	pBaseLayer->SetNormals(pNormalElement);

	for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
	{
		const cd::Direction& normal = mesh.GetVertexNormal(vertexIndex);
		pNormalElement->GetDirectArray().Add(fbxsdk::FbxVector4(normal.x(), normal.y(), normal.z(), 0.0f));
	}

	// Create base color uv.
	if (mesh.GetVertexUVSetCount() > 0U)
	{
		auto* pAlbedoUVElement = fbxsdk::FbxLayerElementUV::Create(pFbxMesh, "Albedo");
		pAlbedoUVElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
		pAlbedoUVElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);
		pBaseLayer->SetUVs(pAlbedoUVElement, fbxsdk::FbxLayerElement::eTextureDiffuse);

		for (uint32_t vertexIndex = 0U; vertexIndex < vertexCount; ++vertexIndex)
		{
			const cd::UV& uv = mesh.GetVertexUV(0U)[vertexIndex];
			pAlbedoUVElement->GetDirectArray().Add(fbxsdk::FbxVector2(uv.x(), 1.0f - uv.y()));
		}
	}

	// Export polygon indices per material.
	pFbxMesh->ReservePolygonCount(polygonCount);
	for (uint32_t materialIndex = 0U, materialCount = mesh.GetMaterialIDCount(); materialIndex < materialCount; ++materialIndex)
	{
		cd::MaterialID materialID = mesh.GetMaterialID(materialIndex);
		fbxsdk::FbxSurfaceMaterial* pMaterial = ExportMaterial(pScene, pNode, materialID, pSceneDatabase);
		assert(pMaterial); // If nullptr, may cause a wrong mapping result.
		uint32_t actualMaterialIndex = pNode->AddMaterial(pMaterial);

		const cd::PolygonGroup& polygonGroup = mesh.GetPolygonGroup(materialIndex);
		for (const auto& polygon : polygonGroup)
		{
			assert(polygon.size() >= 3U);
			pFbxMesh->BeginPolygon(actualMaterialIndex);
			for (auto vertexID : polygon)
			{
				pFbxMesh->AddPolygon(vertexID.Data());
			}
			pFbxMesh->EndPolygon();
		}
	}
}

void FbxConsumerImpl::ExportNodeRecursively(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pParentNode, cd::NodeID nodeID, const cd::SceneDatabase* pSceneDatabase)
{
	if (!nodeID.IsValid() || nodeID.Data() >= pSceneDatabase->GetNodeCount())
	{
		return;
	}

	const auto& node = pSceneDatabase->GetNode(nodeID.Data());
	fbxsdk::FbxNode* pNode = ExportNode(pScene, node.GetName(), node.GetTransform(), pSceneDatabase);
	if (pParentNode)
	{
		pParentNode->AddChild(pNode);
	}

	for (cd::NodeID childID : node.GetChildIDs())
	{
		ExportNodeRecursively(pScene, pNode, childID, pSceneDatabase);
	}
}

fbxsdk::FbxNode* FbxConsumerImpl::ExportNode(fbxsdk::FbxScene* pScene, const char* pName, const cd::Transform& transform, const cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxNode* pNode = pScene->FindNodeByName(pName);
	if (!pNode)
	{
		pNode = fbxsdk::FbxNode::Create(pScene, pName);
		pNode->SetShadingMode(fbxsdk::FbxNode::EShadingMode::eTextureShading);

		const auto& translation = transform.GetTranslation();
		const auto& rotation = transform.GetRotation();
		const auto& scale = transform.GetScale();
		pNode->LclTranslation.Set(fbxsdk::FbxVector4(translation.x(), translation.y(), translation.z()));
		pNode->LclRotation.Set(fbxsdk::FbxVector4(rotation.Roll(), rotation.Pitch(), rotation.Yaw()));
		pNode->LclScaling.Set(fbxsdk::FbxVector4(scale.x(), scale.y(), scale.z()));
	}

	return pNode;
}

bool FbxConsumerImpl::ExportFbxFile(fbxsdk::FbxScene* pScene)
{
	auto* pExporter = fbxsdk::FbxExporter::Create(m_pSDKManager, "");

	int fileFormat;
	if (IsOptionEnabled(FbxConsumerOptions::SaveAsASCII))
	{
		fileFormat = m_pSDKManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");
	}
	else
	{
		fileFormat = m_pSDKManager->GetIOPluginRegistry()->GetNativeWriterFormat();
	}

	pExporter->SetFileExportVersion(FBX_2018_00_COMPATIBLE, fbxsdk::FbxSceneRenamer::eNone);
	if (!pExporter->Initialize(m_filePath.c_str(), fileFormat))
	{
		return false;
	}

	return pExporter->Export(pScene);
}

}