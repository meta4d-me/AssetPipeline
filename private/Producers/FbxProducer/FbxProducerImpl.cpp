#include "FbxProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Scene/MaterialTextureType.h"
#include "Scene/Mesh.h"
#include "Scene/ObjectID.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"

#include <fbxsdk.h>

#include <cassert>
#include <format>
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
		cd::Quaternion::FromPitchYawRoll(static_cast<float>(rotation[0]), static_cast<float>(rotation[1]), static_cast<float>(rotation[2])),
		cd::Vec3f(static_cast<float>(scaling[0]), static_cast<float>(scaling[1]), static_cast<float>(scaling[2])));
}

cd::Matrix4x4 ConvertFbxMatrixToCDMatrix(fbxsdk::FbxAMatrix matrix)
{
	cd::Matrix4x4 cdMatrix(
		static_cast<float>(matrix.Get(0, 0)), static_cast<float>(matrix.Get(0, 1)), static_cast<float>(matrix.Get(0, 2)), static_cast<float>(matrix.Get(0, 3)),
		static_cast<float>(matrix.Get(1, 0)), static_cast<float>(matrix.Get(1, 1)), static_cast<float>(matrix.Get(1, 2)), static_cast<float>(matrix.Get(1, 3)),
		static_cast<float>(matrix.Get(2, 0)), static_cast<float>(matrix.Get(2, 1)), static_cast<float>(matrix.Get(2, 2)), static_cast<float>(matrix.Get(2, 3)),
		static_cast<float>(matrix.Get(3, 0)), static_cast<float>(matrix.Get(3, 1)), static_cast<float>(matrix.Get(3, 2)), static_cast<float>(matrix.Get(3, 3)));
	return cdMatrix;
}

cd::Transform ConvertFbxMatrixToTranform(fbxsdk::FbxAMatrix Matrix)
{
	cd::Matrix4x4 transformMatrix = ConvertFbxMatrixToCDMatrix(Matrix);
	
	return cd::Transform(
		cd::Vec3f(transformMatrix.GetTranslation()),
		cd::Quaternion(cd::Quaternion::FromMatrix(transformMatrix.GetRotation())),
		cd::Vec3f(transformMatrix.GetScale()));
}

fbxsdk::FbxAMatrix GetGeometryTransformation(fbxsdk::FbxNode* pNode)
{
	assert(pNode && "Null for mesh geometry");
	const fbxsdk::FbxVector4& translation = pNode->GetGeometricTranslation(fbxsdk::FbxNode::eSourcePivot);
	const fbxsdk::FbxVector4& rotation = pNode->GetGeometricRotation(fbxsdk::FbxNode::eSourcePivot);
	const fbxsdk::FbxVector4& scale = pNode->GetGeometricScaling(fbxsdk::FbxNode::eSourcePivot);
	return fbxsdk::FbxAMatrix(translation, rotation, scale);
}

void UnrollRotationCurves(fbxsdk::FbxNode* pNode, fbxsdk::FbxAnimLayer* pAnimationLayer, fbxsdk::FbxAnimCurveFilterUnroll* pUnrollFilter)
{
	if (!pNode)
	{
		return;
	}

	if (fbxsdk::FbxAnimCurveNode* pCurveNode = pNode->LclRotation.GetCurveNode(pAnimationLayer))
	{
		if (uint32_t channelCount = pCurveNode->GetChannelsCount(); channelCount > 0U)
		{
			std::vector<fbxsdk::FbxAnimCurve*> rotationCurves;
			rotationCurves.reserve(channelCount);
			for (uint32_t channelIndex = 0U; channelIndex < channelCount; ++channelIndex)
			{
				rotationCurves.push_back(pCurveNode->GetCurve(channelIndex));
			}

			fbxsdk::FbxEuler::EOrder rotationOrder = eEulerXYZ;
			pNode->GetRotationOrder(FbxNode::eSourcePivot, rotationOrder);
			pUnrollFilter->SetRotationOrder(rotationOrder);
			pUnrollFilter->Apply(rotationCurves.data(), static_cast<int32_t>(rotationCurves.size()));
		}
	}

	for (int32_t nodeIndex = 0; nodeIndex < pNode->GetChildCount(); ++nodeIndex)
	{
		UnrollRotationCurves(pNode->GetChild(nodeIndex), pAnimationLayer, pUnrollFilter);
	}
}

void BakeAnimationLayers(fbxsdk::FbxScene* scene)
{
	// Bake animation layers to the base layer and destroy other layers.
	uint32_t animationStackCount = scene->GetSrcObjectCount<fbxsdk::FbxAnimStack>();
	for (uint32_t stackIndex = 0U; stackIndex < animationStackCount; ++stackIndex)
	{
		fbxsdk::FbxAnimStack* pAnimationStack = scene->GetSrcObject<fbxsdk::FbxAnimStack>(stackIndex);
		if (pAnimationStack->GetMemberCount() > 1)
		{
			// TODO : will check all layers to decide final sample rate.
			constexpr int sampleRate = 30;

			fbxsdk::FbxTime framePeriod;
			framePeriod.SetSecondDouble(1.0 / sampleRate);

			fbxsdk::FbxTimeSpan timeSpan = pAnimationStack->GetLocalTimeSpan();
			pAnimationStack->BakeLayers(scene->GetAnimationEvaluator(), timeSpan.GetStart(), timeSpan.GetStop(), framePeriod);

			fbxsdk::FbxAnimLayer* pAnimationBaseLayer = pAnimationStack->GetMember<fbxsdk::FbxAnimLayer>(0);
			fbxsdk::FbxAnimCurveFilterUnroll unrollFilter;
			unrollFilter.Reset();

			UnrollRotationCurves(scene->GetRootNode(), pAnimationBaseLayer, &unrollFilter);
		}
	}
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

	m_pSDKGeometryConverter = std::make_unique<fbxsdk::FbxGeometryConverter>(m_pSDKManager);
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

	{
		// Query file axis system and unit system.
		//fbxsdk::FbxAxisSystem fileAxisSystem = pSDKScene->GetGlobalSettings().GetAxisSystem();
		//fbxsdk::FbxSystemUnit fileUnitSystem = pSDKScene->GetGlobalSettings().GetSystemUnit();

		//cd::Handedness handedness = fileAxisSystem.GetCoorSystem() == fbxsdk::FbxAxisSystem::eRightHanded ? cd::Handedness::Right : cd::Handedness::Left;
		//auto GetUpVector = [&fileAxisSystem]()
		//{
		//	int sign = 1;
		//	switch (fileAxisSystem.GetUpVector(sign))
		//	{
		//	case fbxsdk::FbxAxisSystem::eXAxis:
		//		return cd::UpVector::XAxis;
		//	case fbxsdk::FbxAxisSystem::eYAxis:
		//		return cd::UpVector::YAxis;
		//	case fbxsdk::FbxAxisSystem::eZAxis:
		//		return cd::UpVector::ZAxis;
		//	}
		//
		//	assert(sign == 1);
		//	return cd::UpVector::YAxis;
		//};
		//
		//int sign;
		//cd::FrontVector frontVector = fileAxisSystem.GetFrontVector(sign) == fbxsdk::FbxAxisSystem::eParityEven ? cd::FrontVector::ParityEven : cd::FrontVector::ParityOdd;
		//assert(sign == 1);

		//cd::AxisSystem axisSystem(handedness, GetUpVector(), frontVector);
		//pSceneDatabase->SetAxisSystem(cd::MoveTemp(axisSystem));
	}

	// Query scene information and prepare to import.
	pSceneDatabase->SetName(m_filePath.c_str());
	m_materialIDGenerator.SetCurrentID(pSceneDatabase->GetMaterialCount());
	m_textureIDGenerator.SetCurrentID(pSceneDatabase->GetTextureCount());
	m_meshIDGenerator.SetCurrentID(pSceneDatabase->GetMeshCount());

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
		TraverseNodeRecursively(pSDKScene->GetRootNode(), cd::NodeID::InvalidID, pSceneDatabase);
	}

	if (WantImportAnimation())
	{
		if (fbxsdk::FbxAnimStack* pAnimStack = pSDKScene->GetSrcObject<FbxAnimStack>(0))
		{
			pSDKScene->SetCurrentAnimationStack(pAnimStack);
			ProcessAnimation(pSDKScene, pSceneDatabase);
		}
	}
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

void FbxProducerImpl::TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute();

	if (nullptr == pNodeAttribute ||
		fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
	{
		AddNode(pSDKNode, parentNodeID, pSceneDatabase);
	}
	else if (fbxsdk::FbxNodeAttribute::eLight == pNodeAttribute->GetAttributeType() && m_importLight)
	{
		const fbxsdk::FbxLight* pFbxLight = reinterpret_cast<const fbxsdk::FbxLight*>(pNodeAttribute);
		assert(pFbxLight);

		AddLight(pFbxLight, pSDKNode->GetName(), details::ConvertFbxNodeTransform(pSDKNode), pSceneDatabase);
	}
	else if (fbxsdk::FbxNodeAttribute::eMesh == pNodeAttribute->GetAttributeType())
	{
		fbxsdk::FbxMesh* pFbxMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pNodeAttribute);
		assert(pFbxMesh);
		
		if (!pFbxMesh->IsTriangleMesh() && IsTriangulateActive())
		{
			const bool bReplace = true;
			fbxsdk::FbxNodeAttribute* pConvertedNode = m_pSDKGeometryConverter->Triangulate(pFbxMesh, bReplace);
			if (pConvertedNode && pConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				pFbxMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pConvertedNode);
				assert(pFbxMesh && pFbxMesh->IsTriangleMesh());
			}
		}
		
		bool hasError = false;
		const fbxsdk::FbxLayer* pMeshBaseLayer = pFbxMesh->GetLayer(0);
		if (!pMeshBaseLayer)
		{
			printf("[Error] No geometry info in the FbxMesh.\n");
			hasError = true;
		}

		if (!hasError)
		{
			const fbxsdk::FbxLayerElementMaterial* pLayerElementMaterial = pMeshBaseLayer->GetMaterials();
			uint32_t materialCount = pSDKNode->GetMaterialCount();

			if (materialCount > 0U)
			{
				fbxsdk::FbxLayerElement::EMappingMode materialMappingMode = pLayerElementMaterial->GetMappingMode();
				if (fbxsdk::FbxLayerElement::eAllSame == materialMappingMode)
				{
					assert(1U == materialCount && "Material is AllSame mapping mode but has multiple materials.");

					AddMesh(pFbxMesh, pSDKNode->GetName(), 0, parentNodeID, pSceneDatabase);
				}
				else if (fbxsdk::FbxLayerElement::eByPolygon == materialMappingMode)
				{
					assert(materialCount > 1U && "Material is ByPolygon mapping mode but only one material.");

					// It will generate multiple meshes to assign one material.
					// To manage them conveniently, they are placed under a new Node.
					cd::NodeID meshesNodeID = AddNode(pSDKNode, parentNodeID, pSceneDatabase);
					for (uint32_t materialIndex = 0U; materialIndex < materialCount; ++materialIndex)
					{
						std::string splitMeshName(std::format("{}_{}", pSDKNode->GetName(), materialIndex));
						AddMesh(pFbxMesh, splitMeshName.c_str(), materialIndex, meshesNodeID, pSceneDatabase);
					}
				}
			}
			else
			{
				AddMesh(pFbxMesh, pSDKNode->GetName(), std::nullopt, parentNodeID, pSceneDatabase);
			}
		}
	}


	for (int childIndex = 0; childIndex < pSDKNode->GetChildCount(); ++childIndex)
	{
		TraverseNodeRecursively(pSDKNode->GetChild(childIndex), parentNodeID, pSceneDatabase);
	}
}

cd::NodeID FbxProducerImpl::AddNode(const fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase)
{
	cd::NodeID nodeID = m_nodeIDGenerator.AllocateID();
	cd::Node node(nodeID, pSDKNode->GetName());
	node.SetTransform(details::ConvertFbxNodeTransform(const_cast<fbxsdk::FbxNode*>(pSDKNode)));
	if (parentNodeID.IsValid())
	{
		pSceneDatabase->GetNode(parentNodeID.Data()).AddChildID(nodeID.Data());
		node.SetParentID(parentNodeID.Data());
	}
	pSceneDatabase->AddNode(cd::MoveTemp(node));

	return nodeID;
}

cd::LightID FbxProducerImpl::AddLight(const fbxsdk::FbxLight* pFbxLight, const char* pLightName, cd::Transform transform, cd::SceneDatabase* pSceneDatabase)
{
	cd::LightType lightType;
	float lightIntensity = static_cast<float>(pFbxLight->Intensity.Get());

	switch (pFbxLight->LightType.Get())
	{
	case fbxsdk::FbxLight::EType::ePoint:
	{
		lightType = cd::LightType::Point;
		break;
	}
	case fbxsdk::FbxLight::EType::eDirectional:
	{
		lightType = cd::LightType::Directional;
		break;
	}
	case fbxsdk::FbxLight::EType::eArea:
	{
		lightIntensity = 1.0f;

		switch (pFbxLight->AreaLightShape.Get())
		{
		case fbxsdk::FbxLight::EAreaLightShape::eRectangle:
		{
			lightType = cd::LightType::Rectangle;
			break;
		}
		default:
		{
			lightType = cd::LightType::Sphere;
			break;
		}
		}
		break;
	}
	case fbxsdk::FbxLight::EType::eSpot:
	{
		lightType = cd::LightType::Spot;
		break;
	}
	default:
	{
		assert("Unknown light source type.\n");
	}
	}

	cd::LightID lightID = m_lightIDGenerator.AllocateID();
	fbxsdk::FbxDouble3 lightColor = pFbxLight->Color.Get();
	cd::Light light(lightID, lightType);
	light.SetName(pLightName);
	light.SetColor(cd::Vec3f(lightColor[0], lightColor[1], lightColor[2]));
	light.SetIntensity(lightIntensity);
	std::pair<float, float> angleScaleAndOffset = light.CalculateScaleAndOffset(static_cast<float>(pFbxLight->InnerAngle.Get()), static_cast<float>(pFbxLight->OuterAngle.Get()));
	light.SetAngleScale(angleScaleAndOffset.first);
	light.SetAngleOffset(angleScaleAndOffset.second);
	light.SetPosition(transform.GetTranslation());
	// TODO: Use AxisSystem to convert.
	light.SetUp(transform.GetRotation() * cd::Vec3f(0.0f, 1.0f, 0.0f));
	light.SetDirection(transform.GetRotation() * cd::Vec3f(1.0f, 0.0f, 0.0f));
	pSceneDatabase->AddLight(cd::MoveTemp(light));

	return lightID;
}

cd::MeshID FbxProducerImpl::AddMesh(const fbxsdk::FbxMesh* pFbxMesh, const char* pMeshName, std::optional<int32_t> optMaterialIndex, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase)
{
	// For geometry data, we only query base layer which means index 0.
	const fbxsdk::FbxLayer* pMeshBaseLayer = pFbxMesh->GetLayer(0);
	uint32_t controlPointCount = pFbxMesh->GetControlPointsCount();
	uint32_t totalPolygonCount = pFbxMesh->GetPolygonCount();

	uint32_t availablePolygonCount = 0U;
	uint32_t availableVertexCount = 0U;
	const fbxsdk::FbxLayerElementMaterial* pLayerElementMaterial = pMeshBaseLayer->GetMaterials();
	bool splitPolygonByMaterial = optMaterialIndex.has_value() && fbxsdk::FbxLayerElement::eByPolygon == pLayerElementMaterial->GetMappingMode();
	if (splitPolygonByMaterial)
	{
		for (uint32_t polygonIndex = 0U; polygonIndex < totalPolygonCount; ++polygonIndex)
		{
			uint32_t polygonVertexCount = pFbxMesh->GetPolygonSize(polygonIndex);
			assert(polygonVertexCount >= 3U);
			int32_t materialIndex = pLayerElementMaterial->GetIndexArray().GetAt(polygonIndex);
			if (materialIndex == optMaterialIndex.value())
			{
				++availablePolygonCount;
				availableVertexCount += polygonVertexCount;
			}
		}
	}
	else
	{
		availablePolygonCount = totalPolygonCount;
		availableVertexCount = pFbxMesh->GetPolygonVertexCount();
	}

	if (0 == availablePolygonCount || 0 == availableVertexCount)
	{
		printf("[Error] Mesh doesn't have any vertex or polygon.\n");
		return cd::MeshID(cd::MeshID::InvalidID);
	}

	// Convert fbx mesh to cd mesh.
	cd::MeshID meshID = m_meshIDGenerator.AllocateID();
	cd::Mesh mesh(meshID, pMeshName, availableVertexCount, availablePolygonCount);
	cd::VertexFormat meshVertexFormat;

	const fbxsdk::FbxVector4* pMeshVertexPositions = pFbxMesh->GetControlPoints();
	const fbxsdk::FbxLayerElementNormal* pLayerElementNormalData = pMeshBaseLayer->GetNormals();
	const fbxsdk::FbxLayerElementTangent* pLayerElementTangentData = pMeshBaseLayer->GetTangents();
	const fbxsdk::FbxLayerElementBinormal* pLayerElementBinormalData = pMeshBaseLayer->GetBinormals();
	const fbxsdk::FbxLayerElementVertexColor* pLayerElementColorData = pMeshBaseLayer->GetVertexColors();

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

	meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	if (pLayerElementNormalData)
	{
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);

		if (pLayerElementTangentData)
		{
			meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
		}

		if (pLayerElementBinormalData)
		{
			meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
		}

	}

	if (layerElementUVDatas.size() > 0)
	{
		mesh.SetVertexUVSetCount(static_cast<uint32_t>(layerElementUVDatas.size()));
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}

	// TODO : Multiple vertex color sets if necessary.
	if (pLayerElementColorData)
	{
		mesh.SetVertexColorSetCount(1U);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Vec4f::ValueType>(), cd::Vec4f::Size);
	}

	int32_t skinDeformerCount = pFbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	if (skinDeformerCount > 0)
	{
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneIndex, cd::AttributeValueType::Int16, cd::MaxBoneInfluenceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneWeight, cd::AttributeValueType::Float, cd::MaxBoneInfluenceCount);
	}

	mesh.SetVertexFormat(cd::MoveTemp(meshVertexFormat));

	// Associate mesh id to its parent transform node.
	if (parentNodeID.IsValid())
	{
		pSceneDatabase->GetNode(parentNodeID.Data()).AddMeshID(meshID.Data());
	}

	if (optMaterialIndex.has_value())
	{
		int32_t materialIndex = optMaterialIndex.value();
		auto itMaterialID = m_fbxMaterialIndexToMaterialID.find(materialIndex);
		assert(itMaterialID != m_fbxMaterialIndexToMaterialID.end());
		mesh.SetMaterialID(itMaterialID->second);
	}

	// Process polygon and vertex data.
	uint32_t polygonVertexBeginIndex = 0U;
	uint32_t polygonVertexEndIndex = 0U;
	uint32_t polygonID = 0U;
	uint32_t currentPolygonCount = 0U;
	std::map<uint32_t, uint32_t> mapVertexIDToControlPointIndex;
	std::map<uint32_t, std::vector<uint32_t>> mapControlPointIndexToVertexID;
	for (uint32_t polygonIndex = 0U; polygonIndex < totalPolygonCount; ++polygonIndex)
	{
		if (currentPolygonCount >= availablePolygonCount)
		{
			break;
		}

		if (splitPolygonByMaterial)
		{
			// Skip faces whose assigned material is not the target one.
			int32_t materialIndex = pLayerElementMaterial->GetIndexArray().GetAt(polygonIndex);
			if (materialIndex != optMaterialIndex.value())
			{
				continue;
			}
		}

		++currentPolygonCount;

		uint32_t polygonVertexCount = pFbxMesh->GetPolygonSize(polygonIndex);
		polygonVertexBeginIndex = polygonVertexEndIndex;
		polygonVertexEndIndex += polygonVertexCount;

		// Position
		cd::Polygon polygon;
		polygon.reserve(polygonVertexCount);
		for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
		{
			uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
			fbxsdk::FbxVector4 position = pMeshVertexPositions[controlPointIndex];
			uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
			mesh.SetVertexPosition(vertexID, cd::Point(position[0], position[1], position[2]));
			polygon.push_back(vertexID);
			mapVertexIDToControlPointIndex[vertexID] = controlPointIndex;
			mapControlPointIndexToVertexID[controlPointIndex].push_back(vertexID);
		}
		mesh.SetPolygon(polygonID++, cd::MoveTemp(polygon));

		// Normal
		bool applyTangentData = false;
		bool applyBinormalData = false;
		if (pLayerElementNormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t normalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementNormalData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t normalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementNormalData->GetReferenceMode() ? normalMapIndex : pLayerElementNormalData->GetIndexArray().GetAt(normalMapIndex);
				fbxsdk::FbxVector4 normalValue = pLayerElementNormalData->GetDirectArray().GetAt(normalValueIndex);
				uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
				mesh.SetVertexNormal(vertexID, cd::Direction(normalValue[0], normalValue[1], normalValue[2]));

				// If normal data exists, apply TBN data.
				applyTangentData = pLayerElementTangentData && pLayerElementBinormalData;
				applyBinormalData = applyTangentData;
			}
		}

		if (applyTangentData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t tangentMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementTangentData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t tangentValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementTangentData->GetReferenceMode() ? tangentMapIndex : pLayerElementTangentData->GetIndexArray().GetAt(tangentMapIndex);
				fbxsdk::FbxVector4 tangentValue = pLayerElementTangentData->GetDirectArray().GetAt(tangentValueIndex);
				uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
				mesh.SetVertexTangent(vertexID, cd::Direction(tangentValue[0], tangentValue[1], tangentValue[2]));
			}
		}

		if (applyBinormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t binormalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementBinormalData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t binormalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementBinormalData->GetReferenceMode() ? binormalMapIndex : pLayerElementBinormalData->GetIndexArray().GetAt(binormalMapIndex);
				fbxsdk::FbxVector4 binormalValue = pLayerElementBinormalData->GetDirectArray().GetAt(binormalValueIndex);
				uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
				mesh.SetVertexBiTangent(vertexID, cd::Direction(binormalValue[0], binormalValue[1], binormalValue[2]));
			}
		}

		// UV
		if (!layerElementUVDatas.empty())
		{
			for (uint32_t uvSetIndex = 0U; uvSetIndex < layerElementUVDatas.size(); ++uvSetIndex)
			{
				const fbxsdk::FbxLayerElementUV* pLayerElementUVData = layerElementUVDatas[uvSetIndex];
				for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
				{
					uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
					uint32_t uvMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementUVData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
					uint32_t uvValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementUVData->GetReferenceMode() ? uvMapIndex : pLayerElementUVData->GetIndexArray().GetAt(uvMapIndex);
					fbxsdk::FbxVector2 uvValue = pLayerElementUVData->GetDirectArray().GetAt(uvValueIndex);
					uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
					mesh.SetVertexUV(uvSetIndex, vertexID, cd::UV(uvValue[0], uvValue[1]));
				}
			}
		}

		// Color
		if (pLayerElementColorData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t colorMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementColorData->GetMappingMode() ? controlPointIndex : polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t colorValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementColorData->GetReferenceMode() ? colorMapIndex : pLayerElementColorData->GetIndexArray().GetAt(colorMapIndex);
				fbxsdk::FbxColor colorValue = pLayerElementColorData->GetDirectArray().GetAt(colorValueIndex);
				uint32_t vertexID = polygonVertexBeginIndex + polygonVertexIndex;
				mesh.SetVertexColor(0U, vertexID, cd::Color(colorValue.mRed, colorValue.mGreen, colorValue.mBlue, colorValue.mAlpha));
			}
		}
	}

	// BlendShape
	int32_t blendShapeCount = pFbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eBlendShape);
	assert(blendShapeCount <= 1 && "Why use two blendshape in a mesh?");
	for (int32_t blendShapeIndex = 0; blendShapeIndex < blendShapeCount; ++blendShapeIndex)
	{
		const fbxsdk::FbxBlendShape* pBlendShape = static_cast<fbxsdk::FbxBlendShape*>(pFbxMesh->GetDeformer(blendShapeIndex, fbxsdk::FbxDeformer::eBlendShape));
		mesh.SetMorphIDs(AddMorphs(pBlendShape, mesh, mapVertexIDToControlPointIndex, pSceneDatabase));
	}

	// Skin
	std::map<uint32_t, uint32_t> mapVertexIndexToCurrentBoneCount;
	for (int32_t deformerIndex = 0; deformerIndex < skinDeformerCount; ++deformerIndex)
	{
		auto* pSkinDeformer = static_cast<fbxsdk::FbxSkin*>(pFbxMesh->GetDeformer(deformerIndex, fbxsdk::FbxDeformer::eSkin));
		pSceneDatabase->SetBoneCount(pSkinDeformer->GetClusterCount());
		for (int32_t clusterIndex = 0; clusterIndex < pSkinDeformer->GetClusterCount(); ++clusterIndex)
		{
			// Cluster is an intermediate link node which connects bones to mesh shapes.
			fbxsdk::FbxCluster* pCluster = pSkinDeformer->GetCluster(clusterIndex);
			
			bool isBoneReused = false;
			const char* pBoneName = pCluster->GetLink()->GetName();
			cd::BoneID::ValueType boneHash = cd::StringHash<cd::BoneID::ValueType>(pBoneName);
			cd::BoneID boneID = m_boneIDGenerator.AllocateID(boneHash, &isBoneReused);
			if (!isBoneReused)
			{
				cd::Bone bone(boneID, pBoneName);
				fbxsdk::FbxNode* pBoneNode = pCluster->GetLink();
				assert(pBoneNode);

				fbxsdk::FbxAMatrix transformMatrix;
				fbxsdk::FbxAMatrix transformLinkMatrix;
				pCluster->GetTransformLinkMatrix(transformLinkMatrix);
				pCluster->GetTransformMatrix(transformMatrix);

				FbxAMatrix geometryTransform = details::GetGeometryTransformation(pBoneNode);
				assert(geometryTransform.IsIdentity());

				// Multiply pFbxMesh->GetNode()->EvaluateGlobalTransform() for axisTransform.
				fbxsdk::FbxAMatrix globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform * pFbxMesh->GetNode()->EvaluateGlobalTransform();
				
				fbxsdk::FbxTime increment;
				increment.SetSecondDouble(0);

				// Currently, boneTransform is useless. I just store the pose of the first frame of the animation.
				bone.SetTransform(details::ConvertFbxMatrixToTranform(pBoneNode->EvaluateGlobalTransform(increment)));
				bone.SetOffset(details::ConvertFbxMatrixToCDMatrix(globalBindposeInverseMatrix));

				// Set the parent bone
				fbxsdk::FbxNode* pParentNode = pBoneNode->GetParent();
				fbxsdk::FbxNodeAttribute* pNodeAttribute = pParentNode->GetNodeAttribute();
				if (pParentNode && pNodeAttribute)
				{
					if (fbxsdk::FbxNodeAttribute::eSkeleton == pNodeAttribute->GetAttributeType())
					{
						auto* pParentBone = const_cast<cd::Bone*>(pSceneDatabase->GetBoneByName(pParentNode->GetName()));
						assert(pParentBone);
						bone.SetParentID(pParentBone->GetID());
						pParentBone->AddChildID(bone.GetID());
					}
				}

				pSceneDatabase->AddBone(cd::MoveTemp(bone));
			}

			const int32_t vertexCount = pCluster->GetControlPointIndicesCount();
			int* pVertexIndices = pCluster->GetControlPointIndices();
			double* pBoneWeights = pCluster->GetControlPointWeights();
			for (int32_t influencedVertexIndex = 0; influencedVertexIndex < vertexCount; ++influencedVertexIndex)
			{
				uint32_t fbxVertexIndex = pVertexIndices[influencedVertexIndex];
				auto itVertexID = mapControlPointIndexToVertexID.find(fbxVertexIndex);
				assert(itVertexID != mapControlPointIndexToVertexID.end());
				for (const auto& vertexIndex : itVertexID->second)
				{
					double weight = pBoneWeights[influencedVertexIndex];
					uint32_t currentBoneCount = 0U;
					auto itVertexBoneCount = mapVertexIndexToCurrentBoneCount.find(vertexIndex);
					if (itVertexBoneCount != mapVertexIndexToCurrentBoneCount.end())
					{
						currentBoneCount = mapVertexIndexToCurrentBoneCount[vertexIndex] + 1;
						assert(currentBoneCount <= cd::MaxBoneInfluenceCount);
					}
					mapVertexIndexToCurrentBoneCount[vertexIndex] = currentBoneCount;
					mesh.SetVertexBoneWeight(currentBoneCount, vertexIndex, boneID, static_cast<float>(weight));
				}
			}
		}
	}
	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
	return meshID;
}

std::vector<cd::MorphID> FbxProducerImpl::AddMorphs(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, const std::map<uint32_t, uint32_t>& mapVertexIDToControlPointIndex, cd::SceneDatabase* pSceneDatabase)
{
	assert(pBlendShape);

	std::vector<cd::MorphID> morphIDs;

	uint32_t sourceVertexCount = sourceMesh.GetVertexCount();
	int32_t blendShapeChannelCount = pBlendShape->GetBlendShapeChannelCount();
	for (int32_t channelIndex = 0; channelIndex < blendShapeChannelCount; ++channelIndex)
	{
		const fbxsdk::FbxBlendShapeChannel* pChannel = static_cast<const fbxsdk::FbxBlendShapeChannel*>(pBlendShape->GetBlendShapeChannel(channelIndex));
		assert(pChannel);

		int32_t targetShapeCount = pChannel->GetTargetShapeCount();
		for (int32_t targetShapeIndex = 0; targetShapeIndex < targetShapeCount; ++targetShapeIndex)
		{
			const fbxsdk::FbxShape* pTargetShape = pChannel->GetTargetShape(targetShapeIndex);
			assert(pTargetShape);

			uint32_t targetShapeVertexID = 0U;
			for (uint32_t sourceVertexID = 0U; sourceVertexID < sourceVertexCount; ++sourceVertexID)
			{
				auto itControlPointIndex = mapVertexIDToControlPointIndex.find(sourceVertexID);
				assert(itControlPointIndex != mapVertexIDToControlPointIndex.end());
				uint32_t controlPointIndex = itControlPointIndex->second;
				const cd::Point& sourceShapePosition = sourceMesh.GetVertexPosition(sourceVertexID);
				fbxsdk::FbxVector4 sdkTargetPos = pTargetShape->GetControlPointAt(controlPointIndex);
				cd::Point targetShapePosition(static_cast<float>(sdkTargetPos[0]), static_cast<float>(sdkTargetPos[1]), static_cast<float>(sdkTargetPos[2]));
				if (sourceShapePosition == targetShapePosition)
				{
					// No difference.
					continue;
				}

				++targetShapeVertexID;
			}

			if (0U == targetShapeVertexID)
			{
				// Target shape is same to source shape.
				continue;
			}

			cd::Morph morph(m_morphIDGenerator.AllocateID(), sourceMesh.GetID(), pTargetShape->GetName(), targetShapeVertexID);
			morphIDs.push_back(morph.GetID());

			uint32_t targetShapeVertexIndex = 0U;
			for (uint32_t sourceVertexID = 0U; sourceVertexID < sourceVertexCount; ++sourceVertexID)
			{
				auto itControlPointIndex = mapVertexIDToControlPointIndex.find(sourceVertexID);
				assert(itControlPointIndex != mapVertexIDToControlPointIndex.end());
				uint32_t controlPointIndex = itControlPointIndex->second;
				const cd::Point& sourceShapePosition = sourceMesh.GetVertexPosition(sourceVertexID);
				fbxsdk::FbxVector4 sdkTargetPos = pTargetShape->GetControlPointAt(controlPointIndex);
				cd::Point targetShapePosition(static_cast<float>(sdkTargetPos[0]), static_cast<float>(sdkTargetPos[1]), static_cast<float>(sdkTargetPos[2]));
				if (sourceShapePosition == targetShapePosition)
				{
					// No difference.
					continue;
				}

				// TODO : import initial weights.
				morph.SetWeight(0.0f);
				morph.SetVertexSourceID(targetShapeVertexIndex, sourceVertexID);
				morph.SetVertexPosition(targetShapeVertexIndex, targetShapePosition);
				++targetShapeVertexIndex;
			}

			pSceneDatabase->AddMorph(cd::MoveTemp(morph));
		}
	}

	return morphIDs;
}

void FbxProducerImpl::AddMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial)
{
	// TODO
	pSDKMaterial;
	pPropertyName;
	pMaterial;
}

void FbxProducerImpl::AddMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase)
{
	if (material.IsTextureSetup(textureType))
	{
		return;
	}

	uint32_t unsupportedTextureCount = sdkProperty.GetSrcObjectCount<fbxsdk::FbxLayeredTexture>() + sdkProperty.GetSrcObjectCount<fbxsdk::FbxProceduralTexture>();
	if (unsupportedTextureCount > 0U)
	{
		printf("UnsupportedTextureCount = %d\n", unsupportedTextureCount);
	}

	uint32_t supportedTextureCount = sdkProperty.GetSrcObjectCount<fbxsdk::FbxFileTexture>();
	for (uint32_t textureIndex = 0U; textureIndex < supportedTextureCount; ++textureIndex)
	{
		fbxsdk::FbxFileTexture* pFileTexture = sdkProperty.GetSrcObject<fbxsdk::FbxFileTexture>(textureIndex);
		if (!pFileTexture)
		{
			continue;
		}

		std::string textureFileName = pFileTexture->GetFileName();
		uint32_t textureHash = cd::StringHash<cd::TextureID::ValueType>(textureFileName);
		bool isReused = false;
		cd::TextureID textureID = m_textureIDGenerator.AllocateID(textureHash, &isReused);
		if (!isReused)
		{
			cd::Texture texture(textureID, pFileTexture->GetName(), textureType);
			texture.SetPath(textureFileName.c_str());
			pSceneDatabase->AddTexture(cd::MoveTemp(texture));
		}
		material.SetTextureID(textureType, textureID);
		material.SetVec2fProperty(textureType, cd::MaterialProperty::UVOffset, cd::Vec2f(0.0f, 0.0f));
		material.SetVec2fProperty(textureType, cd::MaterialProperty::UVScale, cd::Vec2f(1.0f, 1.0f));
	}
}

cd::MaterialID FbxProducerImpl::AddMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase)
{
	uint32_t materialHash = cd::StringHash<cd::MaterialID::ValueType>(pSDKMaterial->GetName());
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash);
	cd::Material material(materialID, pSDKMaterial->GetName(), cd::MaterialType::BasePBR);

	if (WantImportTexture())
	{
		static std::unordered_map<std::string, cd::MaterialTextureType> mapTexturePropertyFBXToCD;
		mapTexturePropertyFBXToCD["base"] = cd::MaterialTextureType::BaseColor;
		mapTexturePropertyFBXToCD["baseColor"] = cd::MaterialTextureType::BaseColor;
		mapTexturePropertyFBXToCD["normalCamera"] = cd::MaterialTextureType::Normal;
		mapTexturePropertyFBXToCD["specularColor"] = cd::MaterialTextureType::Roughness;
		mapTexturePropertyFBXToCD["metalness"] = cd::MaterialTextureType::Metallic;
		mapTexturePropertyFBXToCD["emissionColor"] = cd::MaterialTextureType::Emissive;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sDiffuse] = cd::MaterialTextureType::BaseColor;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sNormalMap] = cd::MaterialTextureType::Normal;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sBump] = cd::MaterialTextureType::Normal;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sSpecularFactor] = cd::MaterialTextureType::Roughness;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sShininess] = cd::MaterialTextureType::Metallic;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sEmissive] = cd::MaterialTextureType::Emissive;
		mapTexturePropertyFBXToCD[fbxsdk::FbxSurfaceMaterial::sAmbient] = cd::MaterialTextureType::Occlusion;
		// mapPropertyFBXToNew[fbxsdk::FbxSurfaceMaterial::sSpecular] = "Specular";
		// mapPropertyFBXToNew[fbxsdk::FbxSurfaceMaterial::sTransparentColor] = "Opacity";
		// mapPropertyFBXToNew[fbxsdk::FbxSurfaceMaterial::sTransparencyFactor] = "OpacityMask";

		fbxsdk::FbxProperty currentProperty = pSDKMaterial->GetFirstProperty();
		while (currentProperty.IsValid())
		{
			const char* pFBXPropertyName = currentProperty.GetNameAsCStr();
			if (const auto& itPropertyName = mapTexturePropertyFBXToCD.find(pFBXPropertyName); itPropertyName != mapTexturePropertyFBXToCD.end())
			{
				AddMaterialTexture(currentProperty, itPropertyName->second, material, pSceneDatabase);
			}

			currentProperty = pSDKMaterial->GetNextProperty(currentProperty);
		}
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));

	return materialID;
}

cd::BoneID FbxProducerImpl::AddBone(const fbxsdk::FbxNode* pSDKNode, cd::BoneID parentBoneID, cd::SceneDatabase* pSceneDatabase)
{
	cd::BoneID boneID = m_boneIDGenerator.AllocateID();
	cd::Bone bone(boneID, pSDKNode->GetName());
	bone.SetTransform(details::ConvertFbxNodeTransform(const_cast<fbxsdk::FbxNode*>(pSDKNode)));
	if (parentBoneID.IsValid())
	{
		pSceneDatabase->GetBone(parentBoneID.Data()).AddChildID(boneID.Data());
		bone.SetParentID(parentBoneID.Data());
	}
	pSceneDatabase->AddBone(cd::MoveTemp(bone));

	return boneID;
}

void FbxProducerImpl::ProcessAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase)
{	
	const int animationCount = scene->GetSrcObjectCount<fbxsdk::FbxAnimStack>();
	for (int animationIndex = 0; animationIndex < animationCount; ++animationIndex)
	{
		// Single-take animation information.
		fbxsdk::FbxAnimStack* pCurrentAnimStack = scene->GetSrcObject<fbxsdk::FbxAnimStack>(animationIndex);
		scene->SetCurrentAnimationStack(pCurrentAnimStack);
		fbxsdk::FbxString animStackName = pCurrentAnimStack->GetName();
		std::string animationName = animStackName.Buffer();
		float start = static_cast<float>(pCurrentAnimStack->GetLocalTimeSpan().GetStart().GetSecondDouble());
		float end = static_cast<float>(pCurrentAnimStack->GetLocalTimeSpan().GetStop().GetSecondDouble());

		cd::AnimationID::ValueType animationHash = cd::StringHash<cd::AnimationID::ValueType>(animationName);
		cd::AnimationID animationID = m_animationIDGenerator.AllocateID(animationHash);
		cd::Animation animation(animationID, cd::MoveTemp(animationName));

		animation.SetDuration(static_cast<float>(end > start ? end - start : 1.0f));
		animation.SetTicksPerSecond(static_cast<float>(24.0f));

		float period = 1.f / 24.f; // todo: it can make variable, like 24fps or 60fps...
		const auto& bones = pSceneDatabase->GetBones();
		int trackCount = static_cast<int>(std::ceil((end - start) / period));
		for (uint32_t boneIndex = 0U; boneIndex < pSceneDatabase->GetBoneCount(); ++boneIndex)
		{
			fbxsdk::FbxNode* bone = scene->FindNodeByName(bones[boneIndex].GetName());
			cd::TrackID::ValueType trackHash = cd::StringHash<cd::TrackID::ValueType>(bones[boneIndex].GetName());
			cd::TrackID trackID = m_trackIDGenerator.AllocateID(trackHash);

			cd::Track boneTrack(trackID, bones[boneIndex].GetName());
			const char* name = bones[boneIndex].GetName();
			boneTrack.SetTranslationKeyCount(trackCount);
			boneTrack.SetRotationKeyCount(trackCount);
			boneTrack.SetScaleKeyCount(trackCount);

			for (int trackIndex = 0; trackIndex < trackCount; ++trackIndex)
			{	
				fbxsdk::FbxTime frameTime;
				frameTime.SetSecondDouble(trackIndex * period);
				fbxsdk::FbxAMatrix currentTransform = bone->EvaluateGlobalTransform(frameTime);
				cd::Transform transformKey = details::ConvertFbxMatrixToTranform(currentTransform);
				
				float time = static_cast<float>(trackIndex * period);

				// Translation.
				auto& cdTranslationKey = boneTrack.GetTranslationKeys()[trackIndex];
				cdTranslationKey.SetTime(time);
				cdTranslationKey.SetValue(transformKey.GetTranslation());

				// Rotation.
				auto& cdRotationKey = boneTrack.GetRotationKeys()[trackIndex];
				cdRotationKey.SetTime(time);
				cdRotationKey.SetValue(transformKey.GetRotation());

				// Scale.
				auto& cdScaleKey = boneTrack.GetScaleKeys()[trackIndex];
				cdScaleKey.SetTime(time);
				cdScaleKey.SetValue(transformKey.GetScale());
			}

			animation.AddBoneTrackID(trackID.Data());
			pSceneDatabase->AddTrack(cd::MoveTemp(boneTrack));
		}

		pSceneDatabase->AddAnimation(cd::MoveTemp(animation));
	}
}
	
}
