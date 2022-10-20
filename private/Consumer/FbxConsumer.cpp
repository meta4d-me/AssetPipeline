#include "FBXConsumer.h"

#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

// C/C++
#include <cassert>

namespace cdtools
{

FbxConsumer::~FbxConsumer()
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

void FbxConsumer::Execute(const SceneDatabase* pSceneDatabase)
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
	m_pSDKScene = fbxsdk::FbxScene::Create(m_pSDKManager, pSceneDatabase->GetName().c_str());

	// Build fbx scene by converting SceneDatabase
	for (uint32_t meshIndex = 0; meshIndex < pSceneDatabase->GetMeshCount(); ++meshIndex)
	{
		const Mesh& mesh = pSceneDatabase->GetMesh(meshIndex);
		
		fbxsdk::FbxMesh* pFbxMesh = fbxsdk::FbxMesh::Create(m_pSDKScene, mesh.GetName().c_str());
		pFbxMesh->InitControlPoints(mesh.GetVertexCount());

		fbxsdk::FbxGeometryElementNormal* pNormalElement = pFbxMesh->CreateElementNormal();
		assert(pNormalElement && "Failed to create fbx normal element.");
		pNormalElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByControlPoint);
		pNormalElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eDirect);

		fbxsdk::FbxGeometryElementUV* pUVElement = nullptr;
		if(mesh.GetVertexUVSetCount() > 0)
		{
			pUVElement = pFbxMesh->CreateElementUV("BaseUV");
			assert(pUVElement && "Failed to create fbx uv element.");
			pUVElement->SetMappingMode(fbxsdk::FbxGeometryElement::eByPolygonVertex);
			pUVElement->SetReferenceMode(fbxsdk::FbxGeometryElement::eIndexToDirect);
		}

		fbxsdk::FbxVector4* pFbxVertices = pFbxMesh->GetControlPoints();
		for(uint32_t vertexIndex = 0; vertexIndex < mesh.GetVertexCount(); ++vertexIndex)
		{
			const Point& position = mesh.GetVertexPosition(vertexIndex);
			pFbxVertices[vertexIndex].Set(position.x(), position.y(), position.z(), 1.0);

			const Direction& normal = mesh.GetVertexNormal(vertexIndex);
			pNormalElement->GetDirectArray().Add(fbxsdk::FbxVector4(normal.x(), normal.y(), normal.z(), 0.0));

			// Only the first channel now
			if(pUVElement)
			{
				const UV& uv = mesh.GetVertexUV(0)[vertexIndex];
				pUVElement->GetDirectArray().Add(fbxsdk::FbxVector2(uv.x(), uv.y()));
			}
		}

		pFbxMesh->ReservePolygonCount(mesh.GetPolygonCount());
		for (uint32_t polygonIndex = 0; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
		{
			const Mesh::Polygon& polygon = mesh.GetPolygon(polygonIndex);

			assert(3 == polygon.GetVertexCount() && "Only support triangles now.");
			pFbxMesh->BeginPolygon(-1, -1, -1, false);
			pFbxMesh->AddPolygon(polygon.v0.Data());
			pFbxMesh->AddPolygon(polygon.v1.Data());
			pFbxMesh->AddPolygon(polygon.v2.Data());
			pFbxMesh->EndPolygon();
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