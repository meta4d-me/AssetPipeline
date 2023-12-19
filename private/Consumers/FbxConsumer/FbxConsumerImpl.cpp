#include "FbxConsumerImpl.h"

#include "Scene/Mesh.h"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

// C/C++
#include <cassert>

namespace cdtools
{

FbxConsumerImpl::~FbxConsumerImpl()
{
	if(m_pSDKScene)
	{
		m_pSDKScene->Destroy();
	}

	if(m_pSDKExporter)
	{
		m_pSDKExporter->Destroy();
	}
	
	if(m_pSDKManager)
	{
		m_pSDKManager->Destroy();
	}
}

void FbxConsumerImpl::Execute(const cd::SceneDatabase* pSceneDatabase)
{
	// Init settings
	m_pSDKManager = fbxsdk::FbxManager::Create();
	fbxsdk::FbxIOSettings* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, "IOSROOT");
	pIOSettings->SetBoolProp(EXP_FBX_MODEL, true);
	pIOSettings->SetBoolProp(EXP_FBX_MATERIAL, false);
	pIOSettings->SetBoolProp(EXP_FBX_TEXTURE, false);
	pIOSettings->SetBoolProp(EXP_FBX_EMBEDDED, false);
	m_pSDKManager->SetIOSettings(pIOSettings);

	// Scene
	m_pSDKScene = fbxsdk::FbxScene::Create(m_pSDKManager, pSceneDatabase->GetName());

	// Build fbx scene by converting SceneDatabase
	for (const auto& mesh : pSceneDatabase->GetMeshes())
	{
		fbxsdk::FbxMesh* pFbxMesh = fbxsdk::FbxMesh::Create(m_pSDKScene, mesh.GetName());
		pFbxMesh->InitControlPoints(mesh.GetVertexCount());

		fbxsdk::FbxGeometryElementNormal* pNormalElement = pFbxMesh->CreateElementNormal();
		assert(pNormalElement && "Failed to create fbx normal element.");
		pNormalElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
		pNormalElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);

		fbxsdk::FbxGeometryElementUV* pUVElement = nullptr;
		if(mesh.GetVertexUVSetCount() > 0U)
		{
			pUVElement = pFbxMesh->CreateElementUV("BaseUV");
			assert(pUVElement && "Failed to create fbx uv element.");
			pUVElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
			pUVElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);
		}

		fbxsdk::FbxVector4* pFbxVertices = pFbxMesh->GetControlPoints();
		for(uint32_t vertexIndex = 0U; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			const cd::Point& position = mesh.GetVertexPosition(vertexIndex);
			pFbxVertices[vertexIndex].Set(position.x(), position.y(), position.z(), 1.0f);

			const cd::Direction& normal = mesh.GetVertexNormal(vertexIndex);
			pNormalElement->GetDirectArray().Add(fbxsdk::FbxVector4(normal.x(), normal.y(), normal.z(), 0.0f));

			// Only the first channel now
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

		fbxsdk::FbxNode* pFbxNode = fbxsdk::FbxNode::Create(m_pSDKScene, pFbxMesh->GetName());
		pFbxNode->SetNodeAttribute(pFbxMesh);
		pFbxNode->SetShadingMode(fbxsdk::FbxNode::EShadingMode::eWireFrame);

		fbxsdk::FbxVector4 translation(0.0, 0.0, 0.0, 0.0);
		fbxsdk::FbxVector4 rotation(0.0, 0.0, 0.0, 0.0);
		fbxsdk::FbxVector4 scale(1.0, 1.0, 1.0, 1.0);
		pFbxNode->LclTranslation.Set(translation);
		pFbxNode->LclRotation.Set(rotation);
		pFbxNode->LclScaling.Set(scale);
		
		m_pSDKScene->GetRootNode()->AddChild(pFbxNode);
	}

	// Export
	m_pSDKExporter = fbxsdk::FbxExporter::Create(m_pSDKManager, "");
	int fileFormat = m_pSDKManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX ascii (*.fbx)");
	if (m_pSDKExporter->Initialize(m_filePath.c_str(), fileFormat))
	{
		m_pSDKExporter->Export(m_pSDKScene);
	}
	else
	{
		assert("Failed to export fbx scene");
	}
}

}