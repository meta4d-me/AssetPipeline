#include "FbxProducerImpl.h"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>

#include <cassert>
#include <vector>

namespace cdtools
{

FbxProducerImpl::~FbxProducerImpl()
{
	if (m_pSDKManager)
	{
		m_pSDKManager->Destroy();
		m_pSDKManager = nullptr;
	}
}

void FbxProducerImpl::Init()
{
	m_pSDKManager = fbxsdk::FbxManager::Create();
	assert(m_pSDKManager && "Failed to init sdk manager.");

	int sdkMajorVersion = 0;
	int sdkMinorVersion = 0;
	int sdkRevision = 0;
	fbxsdk::FbxManager::GetFileFormatVersion(sdkMajorVersion, sdkMinorVersion, sdkRevision);
	printf("FBXSDK Version : %d, %d, %d\n", sdkMajorVersion, sdkMinorVersion, sdkRevision);

	if (fbxsdk::FbxIOSettings* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, "IOSROOT"))
	{
		pIOSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
		pIOSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
		pIOSettings->SetBoolProp(IMP_FBX_LINK, true);
		pIOSettings->SetBoolProp(IMP_FBX_SHAPE, true);
		pIOSettings->SetBoolProp(IMP_FBX_GOBO, true);
		pIOSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
		pIOSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		m_pSDKManager->SetIOSettings(pIOSettings);
	}
}

void FbxProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	Init();

	fbxsdk::FbxImporter* pSDKImporter = fbxsdk::FbxImporter::Create(m_pSDKManager, "FbxProducer");
	assert(pSDKImporter && "Failed to init sdk importer.");
	if (!pSDKImporter->Initialize(m_filePath.c_str()))
	{
		fbxsdk::FbxString errorInfo = pSDKImporter->GetStatus().GetErrorString();
		printf("Failed to import fbx file : %s", errorInfo.Buffer());
		return;
	}

	fbxsdk::FbxScene* pSDKScene = fbxsdk::FbxScene::Create(m_pSDKManager, "ProducedScene");
	if (!pSDKImporter->Import(pSDKScene))
	{
		fbxsdk::FbxString errorInfo = pSDKImporter->GetStatus().GetErrorString();
		printf("Failed to import fbx model into current scene : %s", errorInfo.Buffer());
		return;
	}

	pSceneDatabase->SetName(m_filePath.c_str());

	// Depth first search start from root node
	TraverseNode(pSDKScene->GetRootNode(), pSceneDatabase);
}

void FbxProducerImpl::TraverseNode(fbxsdk::FbxNode* pSDKNode, cd::SceneDatabase* pSceneDatabase)
{
	if (fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute())
	{
		switch (pNodeAttribute->GetAttributeType())
		{
			case fbxsdk::FbxNodeAttribute::eMesh:
			{
				if(!TraverseMeshNode(pSDKNode, pSceneDatabase))
				{
					printf("Failed to parse mesh node %s", pSDKNode->GetName());
				}
			}
			break;
			case fbxsdk::FbxNodeAttribute::eNull:
			{
				if(!TraverseTransformNode(pSDKNode, pSceneDatabase))
				{
					printf("Failed to parse transform node %s", pSDKNode->GetName());
				}
			}
			break;
			default:
			{
				printf("[Not implemented] node is %s", pSDKNode->GetName());
			}
			break;
		}
	}

	for (int nodeIndex = 0; nodeIndex < pSDKNode->GetChildCount(); ++nodeIndex)
	{
		TraverseNode(pSDKNode->GetChild(nodeIndex), pSceneDatabase);
	}
}

bool FbxProducerImpl::TraverseMeshNode(fbxsdk::FbxNode* pMeshNode, cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxMesh* pFbxMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pMeshNode->GetNodeAttribute());
	assert(pFbxMesh && "Invalid node attribute for mesh node");

	cd::Mesh mesh(cd::MeshID(m_nodeIDCounter++), pMeshNode->GetName(), pFbxMesh->GetPolygonVertexCount(), pFbxMesh->GetPolygonCount());
	mesh.SetVertexColorSetCount(pFbxMesh->GetElementVertexColorCount());
	mesh.SetVertexUVSetCount(pFbxMesh->GetElementUVCount());

	fbxsdk::FbxAMatrix localTransform = pMeshNode->EvaluateLocalTransform();
	localTransform.GetT();
	localTransform.GetR();
	localTransform.GetS();

	fbxsdk::FbxVector4* pVertexPositions = pFbxMesh->GetControlPoints();
	for (uint32_t polygonIndex = 0, vertexID = 0; polygonIndex < mesh.GetPolygonCount(); ++polygonIndex)
	{
		if(3 != pFbxMesh->GetPolygonSize(polygonIndex))
		{
			assert("Need to triangulate polygon");
			continue;
		}

		mesh.SetPolygon(polygonIndex, cd::VertexID(vertexID), cd::VertexID(vertexID + 1), cd::VertexID(vertexID + 2));
		
		for(uint32_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			int controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, vertexIndex);

			// position
			const fbxsdk::FbxVector4& vertexPosition = pVertexPositions[controlPointIndex];
			mesh.SetVertexPosition(vertexID, cd::Point(vertexPosition[0], vertexPosition[1], vertexPosition[2]));

			// normal
			if(fbxsdk::FbxGeometryElementNormal* pVertexNormalSet = pFbxMesh->GetElementNormal(0))
			{
				fbxsdk::FbxVector4 normal;
				fbxsdk::FbxLayerElement::EMappingMode mappingMode = pVertexNormalSet->GetMappingMode();
				fbxsdk::FbxLayerElement::EReferenceMode refrenceMode = pVertexNormalSet->GetReferenceMode();
				if (fbxsdk::FbxGeometryElement::eByPolygonVertex == mappingMode)
				{
					int normalIndex;
					if (fbxsdk::FbxGeometryElement::eDirect == refrenceMode)
					{
						normalIndex = vertexID;
					}
					else if (fbxsdk::FbxGeometryElement::eIndexToDirect == refrenceMode)
					{
						normalIndex = pVertexNormalSet->GetIndexArray().GetAt(vertexID);
					}
					else
					{
						assert("Failed to parse vertex normal data.");
					}
					normal = pVertexNormalSet->GetDirectArray().GetAt(normalIndex);
				}
				else if (FbxGeometryElement::eByControlPoint == mappingMode)
				{
					int normalIndex;
					if (fbxsdk::FbxGeometryElement::eDirect == refrenceMode)
					{
						normalIndex = controlPointIndex;
					}
					else if(fbxsdk::FbxGeometryElement::eIndexToDirect == refrenceMode)
					{
						normalIndex = pVertexNormalSet->GetIndexArray().GetAt(controlPointIndex);
					}
					else
					{
						assert("Failed to parse vertex normal data.");
					}

					normal = pVertexNormalSet->GetDirectArray().GetAt(normalIndex);
				}

				mesh.SetVertexNormal(vertexID, cd::Direction(normal[0], normal[1], normal[2]));
			}

			// uv
			for (uint32_t uvSetIndex = 0; uvSetIndex < mesh.GetVertexUVSetCount(); ++uvSetIndex)
			{
				if(fbxsdk::FbxGeometryElementUV* pUVSet = pFbxMesh->GetElementUV(uvSetIndex))
				{
					bool unmappedUV;
					fbxsdk::FbxVector2 uv;
					pFbxMesh->GetPolygonVertexUV(polygonIndex, vertexIndex, pUVSet->GetName(), uv, unmappedUV);
					uv[1] = 1.0f - uv[1];

					mesh.SetVertexUV(uvSetIndex, vertexIndex, cd::UV(uv[0], uv[1]));
				}
			}

			// color
			for (uint32_t colorSetIndex = 0; colorSetIndex < mesh.GetVertexColorSetCount(); ++colorSetIndex)
			{
				fbxsdk::FbxGeometryElementVertexColor* pVertexColorSet = pFbxMesh->GetElementVertexColor(colorSetIndex);
				assert(pVertexColorSet && "Failed to get valid color set.");

				fbxsdk::FbxColor color;
				fbxsdk::FbxLayerElement::EMappingMode mappingMode = pVertexColorSet->GetMappingMode();
				fbxsdk::FbxLayerElement::EReferenceMode refrenceMode = pVertexColorSet->GetReferenceMode();
				if (fbxsdk::FbxGeometryElement::eByControlPoint == mappingMode)
				{
					int colorIndex;
					if (FbxGeometryElement::eDirect == refrenceMode)
					{
						colorIndex = controlPointIndex;
					}
					else if (FbxGeometryElement::eIndexToDirect == refrenceMode)
					{
						colorIndex = pVertexColorSet->GetIndexArray().GetAt(controlPointIndex);
					}
					else
					{
						assert("Failed to parse vertex color data.");
					}

					color = pVertexColorSet->GetDirectArray().GetAt(colorIndex);
				}
				else if (FbxGeometryElement::eByPolygonVertex == mappingMode)
				{
					int colorIndex;
					if (FbxGeometryElement::eDirect == refrenceMode)
					{
						colorIndex = vertexID;
					}
					else if (FbxGeometryElement::eIndexToDirect == refrenceMode)
					{
						colorIndex = pVertexColorSet->GetIndexArray().GetAt(vertexID);
					}
					else
					{
						assert("Failed to parse vertex color data.");
					}

					color = pVertexColorSet->GetDirectArray().GetAt(colorIndex);
				}

				mesh.SetVertexColor(colorSetIndex, vertexIndex, cd::Color(color.mRed, color.mGreen, color.mBlue, color.mAlpha));
			}

			++vertexID;
		}
	}

	pSceneDatabase->AddMesh(std::move(mesh));

	return true;
}

bool FbxProducerImpl::TraverseTransformNode(fbxsdk::FbxNode* pTransformNode, cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxAMatrix localTransform = pTransformNode->EvaluateLocalTransform();
	localTransform.GetT();
	localTransform.GetR();
	localTransform.GetS();

	pSceneDatabase;

	return true;
}

}