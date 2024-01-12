#include "FbxConsumerImpl.h"

#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

#include <cassert>

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
	// Init settings
	m_pSDKManager = fbxsdk::FbxManager::Create();
	auto* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, IOSROOT);
	m_pSDKManager->SetIOSettings(pIOSettings);

	fbxsdk::FbxScene* pScene = CreateScene(pSceneDatabase);
	assert(pScene);

	// Build fbx scene by converting SceneDatabase
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		ExportMesh(pScene, mesh, pSceneDatabase);
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

void FbxConsumerImpl::ExportMesh(fbxsdk::FbxScene* pScene, const cd::Mesh& mesh, const cd::SceneDatabase* pSceneDatabase)
{
	uint32_t controlPointCount = mesh.GetVertexCount();
	assert(controlPointCount > 0U && mesh.GetPolygonCount() > 0U);

	auto* pFbxMesh = fbxsdk::FbxMesh::Create(pScene, mesh.GetName());

	// Export position.
	pFbxMesh->InitControlPoints(controlPointCount);
	fbxsdk::FbxVector4* pFbxVertices = pFbxMesh->GetControlPoints();
	for (uint32_t vertexIndex = 0U; vertexIndex < controlPointCount; ++vertexIndex)
	{
		const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
		pFbxVertices[vertexIndex].Set(position.x(), position.y(), position.z(), 1.0f);
	}

	// Export polygon indices.
	for (const auto& polygonGroup : mesh.GetPolygonGroups())
	{
		for (const auto& polygon : polygonGroup)
		{

		}
	}

	int baseLayerIndex = pFbxMesh->CreateLayer();
	assert(0 == baseLayerIndex);
	fbxsdk::FbxLayer* pBaseLayer = pFbxMesh->GetLayer(baseLayerIndex);

	// Export normal.
	fbxsdk::FbxGeometryElementNormal* pNormalElement = pFbxMesh->CreateElementNormal();
	assert(pNormalElement && "Failed to create fbx normal element.");
	pNormalElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
	pNormalElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);

	fbxsdk::FbxGeometryElementUV* pUVElement = nullptr;
	if (mesh.GetVertexUVSetCount() > 0U)
	{
		pUVElement = pFbxMesh->CreateElementUV("BaseUV");
		assert(pUVElement && "Failed to create fbx uv element.");
		pUVElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
		pUVElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);
	}

	for (uint32_t vertexIndex = 0U; vertexIndex < controlPointCount; ++vertexIndex)
	{
		const cd::Direction& normal = mesh.GetVertexNormal(vertexIndex);
		pNormalElement->GetDirectArray().Add(fbxsdk::FbxVector4(normal.x(), normal.y(), normal.z(), 0.0f));

		if (pUVElement)
		{
			const cd::UV& uv = mesh.GetVertexUV(0)[vertexIndex];
			pUVElement->GetDirectArray().Add(fbxsdk::FbxVector2(uv.x(), uv.y()));
		}
	}


	pFbxMesh->ReservePolygonCount(mesh.GetPolygonCount());
	for (const auto& polygonGroup : mesh.GetPolygonGroups())
	{
		for (const auto& polygon : polygonGroup)
		{
			pFbxMesh->BeginPolygon(-1, -1, -1, false);
			for (uint32_t index = 0U; index < polygon.size(); ++index)
			{
				pFbxMesh->AddPolygon(polygon[index].Data());
			}
			pFbxMesh->EndPolygon();
		}
	}

	auto* pFbxNode = fbxsdk::FbxNode::Create(pScene, pFbxMesh->GetName());
	pFbxNode->SetNodeAttribute(pFbxMesh);
	pFbxNode->SetShadingMode(fbxsdk::FbxNode::EShadingMode::eWireFrame);

	fbxsdk::FbxVector4 translation(0.0, 0.0, 0.0, 0.0);
	fbxsdk::FbxVector4 rotation(0.0, 0.0, 0.0, 0.0);
	fbxsdk::FbxVector4 scale(1.0, 1.0, 1.0, 1.0);
	pFbxNode->LclTranslation.Set(translation);
	pFbxNode->LclRotation.Set(rotation);
	pFbxNode->LclScaling.Set(scale);

	pScene->GetRootNode()->AddChild(pFbxNode);
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