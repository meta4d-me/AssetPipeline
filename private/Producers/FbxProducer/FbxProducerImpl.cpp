#include "FbxProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Scene/Mesh.h"
#include "Scene/ObjectID.h"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>

#include <cassert>
#include <filesystem>
#include <vector>

namespace details
{

cd::Transform ConvertFbxNodeTransform(fbxsdk::FbxNode* pNode)
{
	fbxsdk::FbxDouble3 translation = pNode->LclTranslation.EvaluateValue(FBXSDK_TIME_ZERO);
	fbxsdk::FbxDouble3 rotation = pNode->LclRotation.EvaluateValue(FBXSDK_TIME_ZERO);
	fbxsdk::FbxDouble3 scaling = pNode->LclScaling.EvaluateValue(FBXSDK_TIME_ZERO);
	return cd::Transform(
		cd::Vec3f(static_cast<float>(translation[0]), static_cast<float>(translation[1]), static_cast<float>(translation[2])),
		cd::Quaternion::FromRollPitchYaw(static_cast<float>(rotation[0]), static_cast<float>(-rotation[1]), static_cast<float>(-rotation[2])),
		cd::Vec3f(static_cast<float>(scaling[0]), static_cast<float>(scaling[1]), static_cast<float>(scaling[2])));
}

}

namespace cdtools
{

FbxProducerImpl::FbxProducerImpl(std::string filePath)
	: m_filePath(cd::MoveTemp(filePath))
{
	m_pSDKManager = fbxsdk::FbxManager::Create();
	assert(m_pSDKManager && "Failed to init sdk manager.");

	fbxsdk::FbxIOSettings* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, IOSROOT);
	assert(m_pSDKManager && "Failed to init sdk manager.");
	m_pSDKManager->SetIOSettings(pIOSettings);
}

FbxProducerImpl::~FbxProducerImpl()
{
	if (m_pSDKManager)
	{
		m_pSDKManager->Destroy();
		m_pSDKManager = nullptr;
	}
}

void FbxProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxIOSettings* pIOSettings = m_pSDKManager->GetIOSettings();

	// Query SDK version and initialize Importer and IOSettings for opening file.
	int32_t sdkMajorVersion = 0, sdkMinorVersion = 0, sdkRevision = 0;
	fbxsdk::FbxManager::GetFileFormatVersion(sdkMajorVersion, sdkMinorVersion, sdkRevision);
	printf("FBXSDK Version : %d, %d, %d\n", sdkMajorVersion, sdkMinorVersion, sdkRevision);

	fbxsdk::FbxImporter* pSDKImporter = fbxsdk::FbxImporter::Create(m_pSDKManager, "FbxProducer");
	assert(pSDKImporter && "Failed to init sdk importer.");
	pIOSettings->SetBoolProp(IMP_RELAXED_FBX_CHECK, true);
	if (!pSDKImporter->Initialize(m_filePath.c_str(), -1, pIOSettings))
	{
		if (fbxsdk::FbxStatus::eInvalidFileVersion == pSDKImporter->GetStatus().GetCode())
		{
			printf("Failed to import because the fbx file version is invalid.\n");
			return;
		}
	}

	// Query fbx file version and import file to scene.
	int32_t fileMajorVersion = 0, fileMinorVersion = 0, fileRevision = 0;
	pSDKImporter->GetFileVersion(fileMajorVersion, fileMinorVersion, fileRevision);
	printf("FBXFile Version : %d, %d, %d\n", fileMajorVersion, fileMinorVersion, fileRevision);

	pIOSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
	pIOSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
	pIOSettings->SetBoolProp(IMP_FBX_LINK, true);
	pIOSettings->SetBoolProp(IMP_FBX_SHAPE, true);
	pIOSettings->SetBoolProp(IMP_FBX_GOBO, true);
	pIOSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
	pIOSettings->SetBoolProp(IMP_SKINS, true);
	pIOSettings->SetBoolProp(IMP_DEFORMATION, true);
	pIOSettings->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	pIOSettings->SetBoolProp(IMP_TAKE, true);
	fbxsdk::FbxScene* pSDKScene = fbxsdk::FbxScene::Create(m_pSDKManager, "ProducedScene");
	if (!pSDKImporter->Import(pSDKScene))
	{
		fbxsdk::FbxString errorInfo = pSDKImporter->GetStatus().GetErrorString();
		printf("Failed to import fbx model into current scene : %s", errorInfo.Buffer());
		return;
	}

	// Query scene information and prepare to import.
	pSceneDatabase->SetName(m_filePath.c_str());
	m_materialIDGenerator.SetCurrentID(pSceneDatabase->GetMaterialCount());
	m_textureIDGenerator.SetCurrentID(pSceneDatabase->GetTextureCount());
	m_meshIDGenerator.SetCurrentID(pSceneDatabase->GetMeshCount());

	std::filesystem::path fbxFilePath(m_filePath.c_str());
	m_textureSearchFolders.push_back(fbxFilePath.parent_path().string());

	// Convert fbx materials to cd materials.
	if (WantImportMaterial())
	{
		fbxsdk::FbxArray<fbxsdk::FbxSurfaceMaterial*> sdkMaterials;
		pSDKScene->FillMaterialArray(sdkMaterials);
		for (int32_t materialIndex = 0; materialIndex < sdkMaterials.Size(); ++materialIndex)
		{
			fbxsdk::FbxSurfaceMaterial* pSDKMaterial = sdkMaterials[materialIndex];
			cd::MaterialID materialID = AddMaterial(pSDKMaterial, pSceneDatabase);
			m_fbxMaterialIndexToMaterialID[materialIndex] = materialID.Data();
		}
	}

	// Convert fbx scene nodes/meshes to cd scene nodes/meshes.
	uint32_t sceneNodeCount = static_cast<uint32_t>(GetSceneNodeCount(pSDKScene->GetRootNode()));
	if (sceneNodeCount > 0U)
	{
		uint32_t oldNodeCount = pSceneDatabase->GetNodeCount();
		m_nodeIDGenerator.SetCurrentID(oldNodeCount);
		pSceneDatabase->SetNodeCount(oldNodeCount + sceneNodeCount);
		TraverseNodeRecursively(pSDKScene->GetRootNode(), nullptr, pSceneDatabase);
	}

	// Convert fbx bone/joint to cd scene bone/joint.
	if (WantImportSkinMesh() && WantImportAnimation())
	{
		uint32_t sceneBoneCount = static_cast<uint32_t>(GetSceneBoneCount(pSDKScene->GetRootNode()));
		if (sceneBoneCount > 0U)
		{
			uint32_t oldBoneCount = pSceneDatabase->GetBoneCount();
			m_boneIDGenerator.SetCurrentID(oldBoneCount);
			pSceneDatabase->SetNodeCount(oldBoneCount + sceneBoneCount);
			TraverseBoneRecursively(pSDKScene->GetRootNode(), pSDKScene, nullptr, pSceneDatabase);
		}
	}
}

void FbxProducerImpl::TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase)
{
	const fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute();

	cd::Node* pNode = nullptr;
	if (nullptr == pNodeAttribute ||
		fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
	{
		cd::NodeID nodeID = AddNode(pSDKNode, pParentNode, pSceneDatabase);
		pNode = &pSceneDatabase->GetNodes()[nodeID.Data()];
	}
	else if (fbxsdk::FbxNodeAttribute::eMesh == pNodeAttribute->GetAttributeType())
	{
		AddMesh(pSDKNode, pParentNode, pSceneDatabase);
	}

	for (int childIndex = 0; childIndex < pSDKNode->GetChildCount(); ++childIndex)
	{
		TraverseNodeRecursively(pSDKNode->GetChild(childIndex), pNode, pSceneDatabase);
	}
}

cd::AnimationID FbxProducerImpl::AddAnimation(fbxsdk::FbxNode* pSDKNode, fbxsdk::FbxScene* pSDKScene, cd::SceneDatabase* pSceneDatabase)
{
	uint32_t animationCount = pSDKScene->GetSrcObjectCount<fbxsdk::FbxAnimStack>();
	if (0U == animationCount)
	{
		return cd::AnimationID(cd::AnimationID::InvalidID);
	}

	double frameRate = fbxsdk::FbxTime::GetFrameRate(pSDKScene->GetGlobalSettings().GetTimeMode());
	fbxsdk::FbxTime startTime = FBXSDK_TIME_INFINITE;
	fbxsdk::FbxTime endTime = FBXSDK_TIME_MINUS_INFINITE;
	uint32_t totalKeyCount = 0U;
	auto ConvertAnimationTransformCurve = [&pSDKNode, &startTime, &endTime, &totalKeyCount](fbxsdk::FbxAnimLayer* pAnimationLayer, const fbxsdk::FbxPropertyT<FbxDouble3>& curveProperty, const char* pChannel)
	{
		fbxsdk::FbxAnimCurve* pAnimationTransformCurve = pSDKNode->LclTranslation.GetCurve(pAnimationLayer, pChannel, false);
		uint32_t keyCount = pAnimationTransformCurve->KeyGetCount();
		totalKeyCount = std::max(totalKeyCount, keyCount);
		
		if (1U == totalKeyCount)
		{
			fbxsdk::FbxAnimCurveKey keyValue = pAnimationTransformCurve->KeyGet(0);
			fbxsdk::FbxTime keyTime = keyValue.GetTime();

			startTime = startTime > keyTime ? keyTime : startTime;
			endTime = endTime < keyTime ? keyTime : endTime;
		}
		else if (totalKeyCount > 1U)
		{
			fbxsdk::FbxTimeSpan localAnimatedTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
			pAnimationTransformCurve->GetTimeInterval(localAnimatedTimeSpan);

			fbxsdk::FbxTime localStartTime = localAnimatedTimeSpan.GetStart();
			fbxsdk::FbxTime localEndTime = localAnimatedTimeSpan.GetStop();

			startTime = startTime > localStartTime ? localStartTime : startTime;
			endTime = endTime < localEndTime ? localEndTime : endTime;
		}
	};

	for (uint32_t animationIndex = 0U; animationIndex < animationCount; ++animationIndex)
	{
		fbxsdk::FbxAnimStack* pAnimationStack = pSDKScene->GetSrcObject<fbxsdk::FbxAnimStack>(animationIndex);
		uint32_t animationLayerCount = pAnimationStack->GetMemberCount();
		for (uint32_t layerIndex = 0U; layerIndex < animationLayerCount; ++layerIndex)
		{
			fbxsdk::FbxAnimLayer* pAnimationLayer = static_cast<fbxsdk::FbxAnimLayer*>(pAnimationStack->GetMember(layerIndex));
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_X);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclScaling, FBXSDK_CURVENODE_COMPONENT_Z);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclScaling, FBXSDK_CURVENODE_COMPONENT_Z);
			ConvertAnimationTransformCurve(pAnimationLayer, pSDKNode->LclScaling, FBXSDK_CURVENODE_COMPONENT_Z);
		}
	}

	return cd::AnimationID(cd::AnimationID::InvalidID);
}

void FbxProducerImpl::TraverseBoneRecursively(fbxsdk::FbxNode* pSDKNode, fbxsdk::FbxScene* pSDKScene, cd::Bone* pParentBone, cd::SceneDatabase* pSceneDatabase)
{
	const fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute();

	cd::Bone* pBone = nullptr;
	switch (pNodeAttribute->GetAttributeType())
	{
		case fbxsdk::FbxNodeAttribute::eNull:
		case fbxsdk::FbxNodeAttribute::eSkeleton:
		{
			cd::BoneID boneID = AddBone(pSDKNode, pBone, pSceneDatabase);
			pBone = &pSceneDatabase->GetBones()[boneID.Data()];

			AddAnimation(const_cast<fbxsdk::FbxNode*>(pSDKNode), pSDKScene, pSceneDatabase);
			break;
		}
		default:
			break;
	}

	for (int childIndex = 0; childIndex < pSDKNode->GetChildCount(); ++childIndex)
	{
		TraverseBoneRecursively(pSDKNode->GetChild(childIndex), pSDKScene, pBone, pSceneDatabase);
	}
}

void FbxProducerImpl::AddMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial)
{
	// TODO
	pSDKMaterial;
	pPropertyName;
	pMaterial;
}

void FbxProducerImpl::AddMaterialTexture(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pTextureName,
	cd::MaterialTextureType textureType, cd::Material* pMaterial, cd::SceneDatabase* pSceneDatabase)
{
	if (pMaterial->IsTextureSetup(textureType))
	{
		return;
	}

	fbxsdk::FbxProperty textureProperty = pSDKMaterial->FindProperty(pTextureName);
	if (textureProperty.GetSrcObjectCount<fbxsdk::FbxTexture>() <= 0)
	{
		return;
	}

	fbxsdk::FbxTexture* pSDKTexture = fbxsdk::FbxCast<fbxsdk::FbxTexture>(textureProperty.GetSrcObject<fbxsdk::FbxTexture>(0));
	for (const std::string& textureSearchFolder : m_textureSearchFolders)
	{
		std::filesystem::path textureFilePath(textureSearchFolder);
		textureFilePath.append(pSDKTexture->GetName());

		if (std::filesystem::exists(textureFilePath))
		{
			// TODO : Need a better way to process things about std::filesystem::path and std::string_view
			std::string filePath = textureFilePath.string();
			uint32_t textureHash = cd::StringHash<cd::TextureID::ValueType>(filePath);
			cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash);
			cd::Texture texture(textureID, textureType, filePath.c_str());
			pMaterial->AddTextureID(textureType, textureID);
			pSceneDatabase->AddTexture(cd::MoveTemp(texture));
			break;
		}
	}
}

cd::MaterialID FbxProducerImpl::AddMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase)
{
	uint32_t materialHash = cd::StringHash<cd::MaterialID::ValueType>(pSDKMaterial->GetName());
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash);
	cd::Material material(materialID, pSDKMaterial->GetName(), cd::MaterialType::BasePBR);
	
	if (WantImportTexture())
	{
		AddMaterialTexture(pSDKMaterial, "baseColor", cd::MaterialTextureType::BaseColor, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuse, cd::MaterialTextureType::BaseColor, &material, pSceneDatabase);
		
		AddMaterialTexture(pSDKMaterial, "normalCamera", cd::MaterialTextureType::Normal, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sBump, cd::MaterialTextureType::Normal, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sNormalMap, cd::MaterialTextureType::Normal, &material, pSceneDatabase);

		AddMaterialTexture(pSDKMaterial, "specularRoughness", cd::MaterialTextureType::Roughness, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sSpecularFactor, cd::MaterialTextureType::Roughness, &material, pSceneDatabase);

		AddMaterialTexture(pSDKMaterial, "metalness", cd::MaterialTextureType::Metallic, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sShininess, cd::MaterialTextureType::Metallic, &material, pSceneDatabase);
		
		AddMaterialTexture(pSDKMaterial, "emissionColor", cd::MaterialTextureType::Emissive, &material, pSceneDatabase);
		AddMaterialTexture(pSDKMaterial, fbxsdk::FbxSurfaceMaterial::sEmissive, cd::MaterialTextureType::Emissive, &material, pSceneDatabase);
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));

	return materialID;
}

cd::NodeID FbxProducerImpl::AddNode(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase)
{
	cd::NodeID nodeID = m_nodeIDGenerator.AllocateID();
	cd::Node node(nodeID, pSDKNode->GetName());
	node.SetTransform(details::ConvertFbxNodeTransform(const_cast<fbxsdk::FbxNode*>(pSDKNode)));
	if (pParentNode)
	{
		pParentNode->AddChildID(nodeID.Data());
		node.SetParentID(pParentNode->GetID().Data());
	}
	pSceneDatabase->AddNode(cd::MoveTemp(node));

	return nodeID;
}

cd::BoneID FbxProducerImpl::AddBone(const fbxsdk::FbxNode* pSDKNode, cd::Bone* pParentBone, cd::SceneDatabase* pSceneDatabase)
{
	cd::BoneID boneID = m_boneIDGenerator.AllocateID();
	cd::Bone bone(boneID, pSDKNode->GetName());
	bone.SetTransform(details::ConvertFbxNodeTransform(const_cast<fbxsdk::FbxNode*>(pSDKNode)));
	if (pParentBone)
	{
		pParentBone->AddChildID(boneID.Data());
		bone.SetParentID(pParentBone->GetID().Data());
	}
	pSceneDatabase->AddBone(cd::MoveTemp(bone));

	return boneID;
}

cd::MeshID FbxProducerImpl::AddMesh(const fbxsdk::FbxNode* pSDKNode, cd::Node* pParentNode, cd::SceneDatabase* pSceneDatabase)
{
	const fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute();
	const fbxsdk::FbxMesh* pFbxMesh = reinterpret_cast<const fbxsdk::FbxMesh*>(pNodeAttribute);
	assert(pFbxMesh);

	if (!pFbxMesh->IsTriangleMesh())
	{
		printf("[Error] Mesh is not triangulated.\n");
		return cd::MeshID(cd::MeshID::InvalidID);
	}

	uint32_t vertexCount = pFbxMesh->GetControlPointsCount();
	uint32_t polygonCount = pFbxMesh->GetPolygonCount();
	if (0U == vertexCount || 0U == polygonCount)
	{
		printf("[Error] Mesh doesn't have any vertex or polygon.\n");
		return cd::MeshID(cd::MeshID::InvalidID);
	}

	const fbxsdk::FbxLayer* pMeshBaseLayer = pFbxMesh->GetLayer(0);
	if (!pMeshBaseLayer)
	{
		printf("[Error] No geometry info in the FbxMesh.\n");
		return cd::MeshID(cd::MeshID::InvalidID);
	}

	// For geometry data, we only query base layer which means index 0.
	const fbxsdk::FbxVector4* pMeshVertexPositions = pFbxMesh->GetControlPoints();
	assert(pMeshVertexPositions);

	const fbxsdk::FbxLayerElementNormal* pLayerElementNormalData = pMeshBaseLayer->GetNormals();
	const fbxsdk::FbxLayerElementTangent* pLayerElementTangentData = pMeshBaseLayer->GetTangents();
	const fbxsdk::FbxLayerElementBinormal* pLayerElementBinormalData = pMeshBaseLayer->GetBinormals();
	const fbxsdk::FbxLayerElementVertexColor* pLayerElementColorData = pMeshBaseLayer->GetVertexColors();

	// For uv data, we need to query other layers to get uv sets for lightmap, shadowmap, decay...
	std::vector<const fbxsdk::FbxLayerElementUV*> layerElementUVDatas;
	for (int32_t layerIndex = 0; layerIndex < pFbxMesh->GetLayerCount(); ++layerIndex)
	{
		const fbxsdk::FbxLayer* pFbxMeshLayer = pFbxMesh->GetLayer(layerIndex);
		fbxsdk::FbxArray<const FbxLayerElementUV*> pLayerUVSets = pFbxMeshLayer->GetUVSets();
		for (int32_t uvSetIndex = 0; uvSetIndex < pFbxMeshLayer->GetUVSetCount(); ++uvSetIndex)
		{
			layerElementUVDatas.push_back(pLayerUVSets[uvSetIndex]);
		}
	}

	// Convert fbx mesh to cd mesh.
	// Process basic information data.
	cd::MeshID meshID = m_meshIDGenerator.AllocateID();
	cd::Mesh mesh(meshID, pSDKNode->GetName(), vertexCount, polygonCount);
	mesh.SetVertexUVSetCount(static_cast<uint32_t>(layerElementUVDatas.size()));

	// TODO : Multiple vertex color sets if necessary.
	if (pLayerElementColorData)
	{
		mesh.SetVertexColorSetCount(1U);
	}

	// Associate mesh id to its parent transform node.
	if (pParentNode)
	{
		pParentNode->AddMeshID(meshID.Data());
	}

	// Associate material id.
	uint32_t materialCount = pSDKNode->GetMaterialCount();
	if (materialCount > 0U)
	{
		const fbxsdk::FbxLayerElementMaterial* pLayerElementMaterial = pMeshBaseLayer->GetMaterials();
		// TODO : Support other material mapping modes.
		// assert(fbxsdk::FbxLayerElement::eAllSame == pLayerElementMaterial->GetMappingMode());
		int32_t materialIndex = pLayerElementMaterial->GetIndexArray().GetAt(0);
		auto itMaterialID = m_fbxMaterialIndexToMaterialID.find(materialIndex);
		assert(itMaterialID != m_fbxMaterialIndexToMaterialID.end());
		mesh.SetMaterialID(itMaterialID->second);
	}

	// Process polygon and vertex data.
	uint32_t polygonVertexBeginIndex = 0U;
	uint32_t polygonVertexEndIndex = 0U;
	for (uint32_t polygonIndex = 0U, vertexID = 0U; polygonIndex < polygonCount; ++polygonIndex, vertexID += 3)
	{
		assert(3 == pFbxMesh->GetPolygonSize(polygonIndex));

		polygonVertexBeginIndex = polygonVertexEndIndex;
		polygonVertexEndIndex += 3;

		// Position
		uint32_t polygonVertexID[3];
		for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
		{
			uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
			fbxsdk::FbxVector4 position = pMeshVertexPositions[polygonVertexIndex];
			mesh.SetVertexPosition(controlPointIndex, cd::Point(position[0], position[1], position[2]));

			polygonVertexID[polygonVertexIndex] = controlPointIndex;
		}
		mesh.SetPolygon(polygonIndex, cd::VertexID(polygonVertexID[0]), cd::VertexID(polygonVertexID[1]), cd::VertexID(polygonVertexID[2]));

		// Normal
		bool applyTangentData = false;
		bool applyBinormalData = false;
		if (pLayerElementNormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t normalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementNormalData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t normalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementNormalData->GetReferenceMode() ? normalMapIndex : pLayerElementNormalData->GetIndexArray().GetAt(normalMapIndex);
				fbxsdk::FbxVector4 normalValue = pLayerElementNormalData->GetDirectArray().GetAt(normalValueIndex);
				mesh.SetVertexNormal(controlPointIndex, cd::Direction(normalValue[0], normalValue[1], normalValue[2]));

				// If normal data exists, apply TBN data.
				applyTangentData = pLayerElementTangentData && pLayerElementBinormalData;
				applyBinormalData = applyTangentData;
			}
		}

		if (applyTangentData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t tangentMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementTangentData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t tangentValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementTangentData->GetReferenceMode() ? tangentMapIndex : pLayerElementTangentData->GetIndexArray().GetAt(tangentMapIndex);
				fbxsdk::FbxVector4 tangentValue = pLayerElementTangentData->GetDirectArray().GetAt(tangentValueIndex);
				mesh.SetVertexTangent(controlPointIndex, cd::Direction(tangentValue[0], tangentValue[1], tangentValue[2]));
			}
		}

		if (applyBinormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t binormalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementBinormalData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t binormalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementBinormalData->GetReferenceMode() ? binormalMapIndex : pLayerElementBinormalData->GetIndexArray().GetAt(binormalMapIndex);
				fbxsdk::FbxVector4 binormalValue = pLayerElementBinormalData->GetDirectArray().GetAt(binormalValueIndex);
				mesh.SetVertexBiTangent(controlPointIndex, cd::Direction(binormalValue[0], binormalValue[1], binormalValue[2]));
			}
		}

		// UV
		if (!layerElementUVDatas.empty())
		{
			for (uint32_t uvSetIndex = 0U; uvSetIndex < layerElementUVDatas.size(); ++uvSetIndex)
			{
				const fbxsdk::FbxLayerElementUV* pLayerElementUVData = layerElementUVDatas[uvSetIndex];
				for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
				{
					uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
					uint32_t uvMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementUVData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
					uint32_t uvValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementUVData->GetReferenceMode() ? uvMapIndex : pLayerElementUVData->GetIndexArray().GetAt(uvMapIndex);
					fbxsdk::FbxVector2 uvValue = pLayerElementUVData->GetDirectArray().GetAt(uvValueIndex);
					mesh.SetVertexUV(uvSetIndex, controlPointIndex, cd::UV(uvValue[0], uvValue[1]));
				}
			}
		}

		// Color
		if (pLayerElementColorData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < 3U; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t colorMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementColorData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t colorValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementColorData->GetReferenceMode() ? colorMapIndex : pLayerElementColorData->GetIndexArray().GetAt(colorMapIndex);
				fbxsdk::FbxColor colorValue = pLayerElementColorData->GetDirectArray().GetAt(colorValueIndex);
				mesh.SetVertexColor(0U, controlPointIndex, cd::Color(colorValue.mRed, colorValue.mGreen, colorValue.mBlue, colorValue.mAlpha));
			}
		}
	}

	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));

	return meshID;
}

int FbxProducerImpl::GetSceneNodeCount(const fbxsdk::FbxNode* pSceneNode)
{
	if (0 == pSceneNode->GetChildCount())
	{
		return 1;
	}

	int totalCount = 0;
	for (int childIndex = 0; childIndex < pSceneNode->GetChildCount(); ++childIndex)
	{
		const fbxsdk::FbxNode* pChildNode = pSceneNode->GetChild(childIndex);
		const fbxsdk::FbxNodeAttribute* pNodeAttribute = pSceneNode->GetNodeAttribute();
		if (nullptr == pNodeAttribute ||
			fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
		{
			totalCount += GetSceneNodeCount(pChildNode);
		}
	}

	return totalCount;
}

int FbxProducerImpl::GetSceneBoneCount(const fbxsdk::FbxNode* pSceneNode)
{
	if (0 == pSceneNode->GetChildCount())
	{
		return 1;
	}

	int totalCount = 0;
	for (int childIndex = 0; childIndex < pSceneNode->GetChildCount(); ++childIndex)
	{
		const fbxsdk::FbxNode* pChildNode = pSceneNode->GetChild(childIndex);
		const fbxsdk::FbxNodeAttribute* pNodeAttribute = pSceneNode->GetNodeAttribute();
		if (pNodeAttribute && fbxsdk::FbxNodeAttribute::eSkeleton == pNodeAttribute->GetAttributeType())
		{
			totalCount += GetSceneBoneCount(pChildNode);
		}
	}

	return totalCount;
}

}