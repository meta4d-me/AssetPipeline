#include "FbxProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Scene/SceneDatabase.h"

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
	if (!pNode)
	{
		fbxsdk::FbxAMatrix matrix;
		matrix.SetIdentity();
		return matrix;
	}
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
	// Default import options.
	m_options.Enable(FbxProducerOptions::ImportMaterial);
	m_options.Enable(FbxProducerOptions::ImportTexture);
	m_options.Enable(FbxProducerOptions::Triangulate);

	// Init fbxsdk settings.
	m_pSDKManager = fbxsdk::FbxManager::Create();
	assert(m_pSDKManager && "Failed to init sdk manager.");

	fbxsdk::FbxIOSettings* pIOSettings = fbxsdk::FbxIOSettings::Create(m_pSDKManager, IOSROOT);
	assert(pIOSettings && "Failed to init io settings.");
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

	// Convert fbx materials to cd materials.
	if (IsOptionEnabled(FbxProducerOptions::ImportMaterial))
	{
		fbxsdk::FbxArray<fbxsdk::FbxSurfaceMaterial*> sdkMaterials;
		pSDKScene->FillMaterialArray(sdkMaterials);
		for (int32_t materialIndex = 0; materialIndex < sdkMaterials.Size(); ++materialIndex)
		{
			fbxsdk::FbxSurfaceMaterial* pSDKMaterial = sdkMaterials[materialIndex];
			cd::MaterialID materialID = ParseMaterial(pSDKMaterial, pSceneDatabase);
			assert(materialIndex == materialID.Data());
		}
	}

	// Convert fbx scene nodes/meshes to cd scene nodes/meshes.
	assert(pSDKScene->GetRootNode());
	TraverseNodeRecursively(pSDKScene->GetRootNode(), cd::NodeID::InvalidID, pSceneDatabase);

	if (IsOptionEnabled(FbxProducerOptions::ImportAnimation))
	{
		if (fbxsdk::FbxAnimStack* pAnimStack = pSDKScene->GetSrcObject<FbxAnimStack>(0))
		{
			pSDKScene->SetCurrentAnimationStack(pAnimStack);
			ParseAnimation(pSDKScene, pSceneDatabase);
		}
	}
}

void FbxProducerImpl::TraverseNodeRecursively(fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase)
{
	fbxsdk::FbxNodeAttribute* pNodeAttribute = pSDKNode->GetNodeAttribute();

	if (nullptr == pNodeAttribute ||
		fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
	{
		ParseNode(pSDKNode, parentNodeID, pSceneDatabase);
	}
	else if (fbxsdk::FbxNodeAttribute::eLight == pNodeAttribute->GetAttributeType() && IsOptionEnabled(FbxProducerOptions::ImportLight))
	{
		const fbxsdk::FbxLight* pFbxLight = reinterpret_cast<const fbxsdk::FbxLight*>(pNodeAttribute);
		assert(pFbxLight);

		ParseLight(pFbxLight, pSDKNode->GetName(), details::ConvertFbxNodeTransform(pSDKNode), pSceneDatabase);
	}
	else if (fbxsdk::FbxNodeAttribute::eMesh == pNodeAttribute->GetAttributeType())
	{
		fbxsdk::FbxMesh* pFbxMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pNodeAttribute);
		assert(pFbxMesh);
		
		if (!pFbxMesh->IsTriangleMesh() && IsOptionEnabled(FbxProducerOptions::Triangulate))
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
			cd::Mesh mesh;
			mesh.SetID(m_meshIDGenerator.AllocateID());
			mesh.SetName(pSDKNode->GetName());
			ParseMesh(mesh, pSDKNode, pFbxMesh);

			// Associate mesh id to its parent transform node.
			if (parentNodeID.IsValid())
			{
				pSceneDatabase->GetNode(parentNodeID.Data()).AddMeshID(mesh.GetID());
			}

			// BlendShape
			if (IsOptionEnabled(FbxProducerOptions::ImportBlendShape))
			{
				int32_t blendShapeCount = pFbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eBlendShape);
				mesh.SetBlendShapeIDCount(blendShapeCount);
				for (int32_t blendShapeIndex = 0; blendShapeIndex < blendShapeCount; ++blendShapeIndex)
				{
					const auto* pBlendShape = static_cast<fbxsdk::FbxBlendShape*>(pFbxMesh->GetDeformer(blendShapeIndex, fbxsdk::FbxDeformer::eBlendShape));
					mesh.SetBlendShapeID(blendShapeIndex, ParseBlendShape(pBlendShape, mesh, pSceneDatabase));
				}
			}

			// SkinMesh
			if (IsOptionEnabled(FbxProducerOptions::ImportSkinMesh))
			{
				auto& vertexFormat = mesh.GetVertexFormat();
				vertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneIndex, cd::AttributeValueType::Int16, cd::MaxBoneInfluenceCount);
				vertexFormat.AddAttributeLayout(cd::VertexAttributeType::BoneWeight, cd::AttributeValueType::Float, cd::MaxBoneInfluenceCount);

				int32_t skinDeformerCount = pFbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
				mesh.SetSkinIDCount(skinDeformerCount);
				for (int32_t skinIndex = 0; skinIndex < skinDeformerCount; ++skinIndex)
				{
					const auto* pSkin = static_cast<fbxsdk::FbxSkin*>(pFbxMesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin));
					mesh.SetSkinID(skinIndex, ParseSkin(pSkin, mesh, pSceneDatabase));
				}
			}

			pSceneDatabase->AddMesh(cd::MoveTemp(mesh));
		}
	}


	for (int childIndex = 0; childIndex < pSDKNode->GetChildCount(); ++childIndex)
	{
		TraverseNodeRecursively(pSDKNode->GetChild(childIndex), parentNodeID, pSceneDatabase);
	}
}

cd::NodeID FbxProducerImpl::ParseNode(const fbxsdk::FbxNode* pSDKNode, cd::NodeID parentNodeID, cd::SceneDatabase* pSceneDatabase)
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

cd::LightID FbxProducerImpl::ParseLight(const fbxsdk::FbxLight* pFbxLight, const char* pLightName, cd::Transform transform, cd::SceneDatabase* pSceneDatabase)
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

void FbxProducerImpl::ParseMesh(cd::Mesh& mesh, fbxsdk::FbxNode* pSDKNode, fbxsdk::FbxMesh* pFbxMesh)
{
	uint32_t materialCount = pSDKNode->GetMaterialCount();
	bool isMaterialEmpty = 0U == materialCount;

	fbxsdk::FbxLayer* pMeshBaseLayer = pFbxMesh->GetLayer(0);
	assert(pMeshBaseLayer);

	// Query smoothing group layer and preprocess.
	fbxsdk::FbxLayerElementSmoothing* pSmoothingLayer = pMeshBaseLayer->GetSmoothing();
	auto smoothingReferenceMode = pSmoothingLayer ? pSmoothingLayer->GetReferenceMode() : fbxsdk::FbxLayerElement::EReferenceMode::eDirect;
	auto smoothingMappingMode = pSmoothingLayer ? pSmoothingLayer->GetMappingMode() : fbxsdk::FbxLayerElement::EMappingMode::eNone;
	if (fbxsdk::FbxLayerElement::EMappingMode::eByPolygon == smoothingMappingMode)
	{
		m_pSDKGeometryConverter->ComputeEdgeSmoothingFromPolygonSmoothing(pFbxMesh);
		// Update
		pSmoothingLayer = pMeshBaseLayer->GetSmoothing();
		smoothingReferenceMode = pSmoothingLayer ? pSmoothingLayer->GetReferenceMode() : fbxsdk::FbxLayerElement::EReferenceMode::eDirect;
		smoothingMappingMode = pSmoothingLayer ? pSmoothingLayer->GetMappingMode() : fbxsdk::FbxLayerElement::EMappingMode::eNone;
	}

	// Init vertex position.
	uint32_t controlPointCount = pFbxMesh->GetControlPointsCount();
	uint32_t vertexInstanceCount = pFbxMesh->GetPolygonVertexCount();
	mesh.Init(controlPointCount, vertexInstanceCount);
	for (uint32_t controlPointIndex = 0U; controlPointIndex < controlPointCount; ++controlPointIndex)
	{
		fbxsdk::FbxVector4 position = pFbxMesh->GetControlPointAt(controlPointIndex);
		mesh.SetVertexPosition(controlPointIndex, cd::Point(position[0], position[1], position[2]));
	}

	// Init vertex format.
	cd::VertexFormat meshVertexFormat;
	if (controlPointCount > 0U)
	{
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), cd::Point::Size);
	}

	const fbxsdk::FbxLayerElementNormal* pLayerElementNormalData = pMeshBaseLayer->GetNormals();
	if (pLayerElementNormalData)
	{
		mesh.SetVertexNormalCount(vertexInstanceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	const fbxsdk::FbxLayerElementTangent* pLayerElementTangentData = pMeshBaseLayer->GetTangents();
	if (pLayerElementTangentData)
	{
		mesh.SetVertexTangentCount(vertexInstanceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	const fbxsdk::FbxLayerElementBinormal* pLayerElementBinormalData = pMeshBaseLayer->GetBinormals();
	if (pLayerElementBinormalData)
	{
		mesh.SetVertexBiTangentCount(vertexInstanceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Direction::ValueType>(), cd::Direction::Size);
	}

	const fbxsdk::FbxLayerElementVertexColor* pLayerElementColorData = pMeshBaseLayer->GetVertexColors();
	if (pLayerElementColorData)
	{
		// TODO : Multiple vertex color sets if necessary.
		mesh.SetVertexColorSetCount(1U);
		mesh.GetVertexColors(0U).resize(vertexInstanceCount);
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::Color, cd::GetAttributeValueType<cd::Vec4f::ValueType>(), cd::Vec4f::Size);
	}

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

	if (!layerElementUVDatas.empty())
	{
		mesh.SetVertexUVSetCount(static_cast<uint32_t>(layerElementUVDatas.size()));
		for (uint32_t uvSetIndex = 0U; uvSetIndex < mesh.GetVertexUVSetCount(); ++uvSetIndex)
		{
			mesh.GetVertexUVs(uvSetIndex).resize(vertexInstanceCount);
		}
		meshVertexFormat.AddAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::UV::ValueType>(), cd::UV::Size);
	}
	mesh.SetVertexFormat(cd::MoveTemp(meshVertexFormat));

	// Init material and polygon container.
	fbxsdk::FbxLayerElementMaterial* pLayerElementMaterial = pMeshBaseLayer->GetMaterials();
	auto materialMappingMode = pLayerElementMaterial ? pLayerElementMaterial->GetMappingMode() : fbxsdk::FbxLayerElement::EMappingMode::eNone;
	if (isMaterialEmpty)
	{
		mesh.SetMaterialIDCount(1U);
		mesh.SetMaterialID(0U, cd::MaterialID::Invalid());

		mesh.SetPolygonGroupCount(1U);
	}
	else
	{
		mesh.SetMaterialIDCount(materialCount);
		for (uint32_t materialIndex = 0U; materialIndex < materialCount; ++materialIndex)
		{
			uint32_t materialIndexIndex = pLayerElementMaterial->GetIndexArray().GetAt(materialIndex);
			fbxsdk::FbxSurfaceMaterial* pMaterial = pSDKNode->GetMaterial(materialIndexIndex);
			assert(pMaterial);
			auto [materialID, _] = AllocateMaterialID(pMaterial);
			mesh.SetMaterialID(materialIndex, materialID);
		}

		mesh.SetPolygonGroupCount(materialCount);
	}

	// Init vertex attributes.
	uint32_t polygonCount = pFbxMesh->GetPolygonCount();
	uint32_t polygonVertexBeginIndex = 0U;
	uint32_t polygonVertexEndIndex = 0U;
	for (uint32_t polygonIndex = 0U; polygonIndex < polygonCount; ++polygonIndex)
	{
		uint32_t polygonVertexCount = pFbxMesh->GetPolygonSize(polygonIndex);
		assert(polygonVertexCount >= 3);
		polygonVertexBeginIndex = polygonVertexEndIndex;
		polygonVertexEndIndex += polygonVertexCount;

		// Query material index.
		uint32_t materialIndex = 0U;
		switch (materialMappingMode)
		{
		case fbxsdk::FbxLayerElement::eAllSame:
		{
			materialIndex = pLayerElementMaterial->GetIndexArray().GetAt(0);
			break;
		}
		case fbxsdk::FbxLayerElement::eByPolygon:
		{
			materialIndex = pLayerElementMaterial->GetIndexArray().GetAt(polygonIndex);
			break;
		}
		default:
			break;
		}

		// Indexes.
		cd::Polygon polygon;
		polygon.reserve(polygonVertexCount);
		for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
		{
			uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
			polygon.push_back(controlPointIndex);
			
			uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
			mesh.SetVertexInstanceID(controlPointIndex, vertexInstanceID);
		}

		// Add polygon to according group split by material.
		mesh.GetPolygonGroup(materialIndex).emplace_back(cd::MoveTemp(polygon));

		// Normal
		bool applyTangentData = false;
		bool applyBinormalData = false;
		if (pLayerElementNormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t normalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementNormalData->GetMappingMode() ? controlPointIndex : vertexInstanceID;
				uint32_t normalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementNormalData->GetReferenceMode() ? normalMapIndex : pLayerElementNormalData->GetIndexArray().GetAt(normalMapIndex);
				fbxsdk::FbxVector4 normalValue = pLayerElementNormalData->GetDirectArray().GetAt(normalValueIndex);
				mesh.SetVertexNormal(vertexInstanceID, cd::Direction(normalValue[0], normalValue[1], normalValue[2]));

				// If normal data exists, apply TBN data.
				applyTangentData = pLayerElementTangentData && pLayerElementBinormalData;
				applyBinormalData = applyTangentData;
			}
		}

		// Tangents
		if (applyTangentData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t tangentMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementTangentData->GetMappingMode() ? controlPointIndex : vertexInstanceID;
				uint32_t tangentValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementTangentData->GetReferenceMode() ? tangentMapIndex : pLayerElementTangentData->GetIndexArray().GetAt(tangentMapIndex);
				fbxsdk::FbxVector4 tangentValue = pLayerElementTangentData->GetDirectArray().GetAt(tangentValueIndex);
				mesh.SetVertexTangent(vertexInstanceID, cd::Direction(tangentValue[0], tangentValue[1], tangentValue[2]));
			}
		}

		if (applyBinormalData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t binormalMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementBinormalData->GetMappingMode() ? controlPointIndex : vertexInstanceID;
				uint32_t binormalValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementBinormalData->GetReferenceMode() ? binormalMapIndex : pLayerElementBinormalData->GetIndexArray().GetAt(binormalMapIndex);
				fbxsdk::FbxVector4 binormalValue = pLayerElementBinormalData->GetDirectArray().GetAt(binormalValueIndex);
				mesh.SetVertexBiTangent(vertexInstanceID, cd::Direction(binormalValue[0], binormalValue[1], binormalValue[2]));
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
					uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
					uint32_t uvMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementUVData->GetMappingMode() ? controlPointIndex : vertexInstanceID;
					uint32_t uvValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementUVData->GetReferenceMode() ? uvMapIndex : pLayerElementUVData->GetIndexArray().GetAt(uvMapIndex);
					fbxsdk::FbxVector2 uvValue = pLayerElementUVData->GetDirectArray().GetAt(uvValueIndex);
					mesh.SetVertexUV(uvSetIndex, vertexInstanceID, cd::UV(uvValue[0], uvValue[1]));
				}
			}
		}

		// Color
		if (pLayerElementColorData)
		{
			for (uint32_t polygonVertexIndex = 0U; polygonVertexIndex < polygonVertexCount; ++polygonVertexIndex)
			{
				uint32_t controlPointIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polygonVertexIndex);
				uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
				uint32_t colorMapIndex = fbxsdk::FbxLayerElement::eByControlPoint == pLayerElementColorData->GetMappingMode() ? controlPointIndex : vertexInstanceID;
				uint32_t colorValueIndex = fbxsdk::FbxLayerElement::eDirect == pLayerElementColorData->GetReferenceMode() ? colorMapIndex : pLayerElementColorData->GetIndexArray().GetAt(colorMapIndex);
				fbxsdk::FbxColor colorValue = pLayerElementColorData->GetDirectArray().GetAt(colorValueIndex);
				mesh.SetVertexColor(0U, vertexInstanceID, cd::Color(colorValue.mRed, colorValue.mGreen, colorValue.mBlue, colorValue.mAlpha));
			}
		}
	}
}

cd::BlendShapeID FbxProducerImpl::ParseBlendShape(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase)
{
	assert(pBlendShape);

	cd::BlendShapeID blendShapeID = m_blendShapeIDGenerator.AllocateID();

	cd::BlendShape blendShape;
	blendShape.SetID(blendShapeID);
	blendShape.SetMeshID(sourceMesh.GetID());
	blendShape.SetName(pBlendShape->GetName());

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

			cd::Morph morph;
			morph.SetID(m_morphIDGenerator.AllocateID());
			morph.SetName(pTargetShape->GetName());
			morph.SetWeight(0.0f);
			morph.SetBlendShapeID(blendShape.GetID());
			blendShape.AddMorphID(morph.GetID());

			for (uint32_t sourceVertexID = 0U; sourceVertexID < sourceVertexCount; ++sourceVertexID)
			{
				const cd::Point& sourceShapePosition = sourceMesh.GetVertexPosition(sourceVertexID);
				fbxsdk::FbxVector4 sdkTargetPos = pTargetShape->GetControlPointAt(sourceVertexID);
				cd::Point targetShapePosition(static_cast<float>(sdkTargetPos[0]), static_cast<float>(sdkTargetPos[1]), static_cast<float>(sdkTargetPos[2]));
				if (sourceShapePosition == targetShapePosition)
				{
					// No difference.
					continue;
				}

				morph.AddVertexSourceID(sourceVertexID);
				morph.AddVertexPosition(targetShapePosition);
			}

			pSceneDatabase->AddMorph(cd::MoveTemp(morph));
		}
	}

	pSceneDatabase->AddBlendShape(cd::MoveTemp(blendShape));

	return blendShapeID;
}

cd::SkinID FbxProducerImpl::ParseSkin(const fbxsdk::FbxSkin* pSkin, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase)
{
	assert(pSkin);

	cd::SkinID skinID = m_skinIDGenerator.AllocateID();
	uint32_t influenceBoneCount = pSkin->GetClusterCount();

	cd::Skin skin;
	skin.SetID(skinID);
	skin.SetMeshID(sourceMesh.GetID());
	skin.SetName(pSkin->GetName());
	skin.SetVertexInfluenceBoneIDCount(influenceBoneCount);
	skin.SetVertexBoneIndexCount(sourceMesh.GetVertexPositionCount());
	skin.SetVertexBoneWeightCount(sourceMesh.GetVertexPositionCount());
	for (int32_t skinClusterIndex = 0; skinClusterIndex < pSkin->GetClusterCount(); ++skinClusterIndex)
	{
		const fbxsdk::FbxCluster* pSkinCluster = pSkin->GetCluster(skinClusterIndex);
		assert(pSkinCluster);

		const fbxsdk::FbxNode* pLinkBone = pSkinCluster->GetLink();
		assert(pLinkBone);

		const char* pBoneName = pLinkBone->GetName();

		const int32_t controlPointIndicesCount = pSkinCluster->GetControlPointIndicesCount();
		int* pControlPointIndices = pSkinCluster->GetControlPointIndices();
		double* pBoneWeights = pSkinCluster->GetControlPointWeights();
		for (int32_t controlPointIndex = 0; controlPointIndex < controlPointIndicesCount; ++controlPointIndex)
		{
			uint32_t vertexIndex = pControlPointIndices[controlPointIndex];
			float boneWeight = static_cast<float>(pBoneWeights[controlPointIndex]);

			
		}
	}

	pSceneDatabase->AddSkin(cd::MoveTemp(skin));

	return skinID;
}

std::pair<cd::MaterialID, bool> FbxProducerImpl::AllocateMaterialID(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial)
{
	uint32_t materialHash = cd::StringHash<cd::MaterialID::ValueType>(pSDKMaterial->GetName());
	bool isReused;
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, &isReused);
	return std::make_pair(materialID, isReused);
}

void FbxProducerImpl::ParseMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial)
{
	// TODO
	pSDKMaterial;
	pPropertyName;
	pMaterial;
}

void FbxProducerImpl::ParseMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase)
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
			cd::Texture texture(textureID, pFileTexture->GetName());
			texture.SetPath(textureFileName.c_str());
			pSceneDatabase->AddTexture(cd::MoveTemp(texture));
		}
		material.SetTextureID(textureType, textureID);
		material.SetVec2fProperty(textureType, cd::MaterialProperty::UVOffset, cd::Vec2f(0.0f, 0.0f));
		material.SetVec2fProperty(textureType, cd::MaterialProperty::UVScale, cd::Vec2f(1.0f, 1.0f));
	}
}

cd::MaterialID FbxProducerImpl::ParseMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase)
{
	auto [materialID, isReused] = AllocateMaterialID(pSDKMaterial);	
	if (isReused)
	{
		assert("Duplicated material to parse?");
		return materialID;
	}

	cd::Material material(materialID, pSDKMaterial->GetName(), cd::MaterialType::BasePBR);

	if (IsOptionEnabled(FbxProducerOptions::ImportTexture))
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
				ParseMaterialTexture(currentProperty, itPropertyName->second, material, pSceneDatabase);
			}

			currentProperty = pSDKMaterial->GetNextProperty(currentProperty);
		}
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));

	return materialID;
}

cd::BoneID FbxProducerImpl::ParseBone(const fbxsdk::FbxNode* pSDKNode, cd::BoneID parentBoneID, cd::SceneDatabase* pSceneDatabase)
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

void FbxProducerImpl::ParseAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase)
{	
	// Locates all skeleton nodes in the fbx scene. Some might be nullptr.
	std::vector<FbxNode*> bones;
	bones.reserve(pSceneDatabase->GetBoneCount());
	for (uint32_t boneIndex = 0; boneIndex < pSceneDatabase->GetBoneCount(); boneIndex++)
	{
		const char* pJointName = pSceneDatabase->GetBone(boneIndex).GetName();
		bones.push_back(scene->FindNodeByName(pJointName));
	}

	const uint32_t animationCount = scene->GetSrcObjectCount<fbxsdk::FbxAnimStack>();
	for (uint32_t animationIndex = 0; animationIndex < animationCount; ++animationIndex)
	{
		// Single-take animation information.
		fbxsdk::FbxAnimStack* pCurrentAnimStack = scene->GetSrcObject<fbxsdk::FbxAnimStack>(animationIndex);
		scene->SetCurrentAnimationStack(pCurrentAnimStack);
		const char* pAnimStackName = pCurrentAnimStack->GetName();
		float start = static_cast<float>(pCurrentAnimStack->GetLocalTimeSpan().GetStart().GetSecondDouble());
		float end = static_cast<float>(pCurrentAnimStack->GetLocalTimeSpan().GetStop().GetSecondDouble());

		cd::AnimationID::ValueType animationHash = cd::StringHash<cd::AnimationID::ValueType>(pAnimStackName);
		cd::AnimationID animationID = m_animationIDGenerator.AllocateID(animationHash);
		cd::Animation animation(animationID, cd::MoveTemp(pAnimStackName));

		float period = 1.f / 30.0f; // todo: it can make variable, like 24fps or 60fps...
		uint32_t trackCount = static_cast<uint32_t>(std::ceil((end - start) / period + 1.0f));

		std::vector<float> times;
		times.resize(trackCount);

		std::vector<std::vector<cd::Matrix4x4>> worldMatrices;
		worldMatrices.resize(pSceneDatabase->GetBoneCount());
		for (uint32_t i = 0; i < pSceneDatabase->GetBoneCount(); ++i)
		{
			worldMatrices[i].resize(trackCount);
		}

		animation.SetDuration(end - start);
		animation.SetTicksPerSecond(static_cast<float>(30.0f));

		// Goes through the whole timeline to compute animated word matrices.
		// Fbx sdk seems to compute nodes transformation for the whole scene, so it's
		// much faster to query all nodes at once for the same time t.
		fbxsdk::FbxAnimEvaluator* evaluator = scene->GetAnimationEvaluator();
		for (uint32_t trackIndex = 0; trackIndex < trackCount; ++trackIndex)
		{
			const float time = std::min(trackIndex * period, end - start);
			times[trackIndex] = time;
			for (uint32_t boneIndex = 0U; boneIndex < pSceneDatabase->GetBoneCount(); ++boneIndex)
			{
				fbxsdk::FbxNode* pBone = bones[boneIndex];
				if (pBone)
				{
					const fbxsdk::FbxAMatrix fbxMatrix = evaluator->GetNodeGlobalTransform(pBone, FbxTimeSeconds(time + start));
					const cd::Matrix4x4 matrix = details::ConvertFbxMatrixToCDMatrix(fbxMatrix);
					worldMatrices[boneIndex][trackIndex] = matrix;
				}

			}
		}

		// Builds world inverse matrices.
		std::vector<std::vector<cd::Matrix4x4>> worldInverseMatrices;
		worldInverseMatrices.resize(pSceneDatabase->GetBoneCount());
		for (uint32_t boneIndex = 0U; boneIndex < pSceneDatabase->GetBoneCount(); ++boneIndex)
		{
		
			const std::vector<cd::Matrix4x4>& boneWorldMatrices = worldMatrices[boneIndex];
			std::vector<cd::Matrix4x4>& boneWorldInvMatrices = worldInverseMatrices[boneIndex];
			boneWorldInvMatrices.resize((trackCount));
			for (uint32_t trackIndex = 0; trackIndex < trackCount; ++trackIndex)
			{
				boneWorldInvMatrices[trackIndex] = boneWorldMatrices[trackIndex].Inverse();
			}
		}

		// Builds local space animation tracks.
		// Allocates all tracks with the same number of joints as the skeleton.
		for (uint32_t boneIndex = 0U; boneIndex < pSceneDatabase->GetBoneCount(); ++boneIndex)
		{
			std::string animationName(pAnimStackName);
			auto trackHash = cd::StringHash<cd::TrackID::ValueType>(animationName + bones[boneIndex]->GetName());
			cd::TrackID trackID = m_trackIDGenerator.AllocateID(trackHash);
			cd::Track boneTrack(trackID, animationName + bones[boneIndex]->GetName());
			boneTrack.SetTranslationKeyCount(trackCount);
			boneTrack.SetRotationKeyCount(trackCount);
			boneTrack.SetScaleKeyCount(trackCount);
			std::vector<cd::Matrix4x4>& boneWorldMatrices = worldMatrices[boneIndex];
			fbxsdk::FbxNode* pParent = bones[boneIndex]->GetParent();
			auto boneID = cd::BoneID::Min();
			if (pParent->GetNodeAttribute()->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
			{
				boneID = pSceneDatabase->GetBoneByName(pParent->GetName())->GetID();
			}
			std::vector<cd::Matrix4x4>& boneWorldInverseMatrices = worldInverseMatrices[boneID.Data()];

			for (uint32_t trackIndex = 0; trackIndex < trackCount; ++trackIndex)
			{
				//Build local matrix;
				cd::Matrix4x4 localMatrix;
				if (pParent->GetNodeAttribute()->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
				{
					localMatrix = boneWorldInverseMatrices[trackIndex] * boneWorldMatrices[trackIndex];
				}
				else
				{
					localMatrix = boneWorldMatrices[trackIndex];
				}
				cd::Transform localTransform = cd::Transform(localMatrix.GetTranslation(),cd::Quaternion::FromMatrix(localMatrix.GetRotation()), localMatrix.GetScale());
				//Fills corresponding track
				const float t = times[trackIndex];
				auto& cdTranslationKey = boneTrack.GetTranslationKeys()[trackIndex];
				cdTranslationKey.SetTime(t);
				cdTranslationKey.SetValue(localMatrix.GetTranslation());

				auto& cdRotationKey = boneTrack.GetRotationKeys()[trackIndex];
				cdRotationKey.SetTime(t);
				cdRotationKey.SetValue(cd::Quaternion::FromMatrix(localMatrix.GetRotation()));

				auto& cdScaleKey = boneTrack.GetScaleKeys()[trackIndex];
				cdScaleKey.SetTime(t);
				cdScaleKey.SetValue(localMatrix.GetScale());
			}
			animation.AddBoneTrackID(trackID.Data());
			pSceneDatabase->AddTrack(cd::MoveTemp(boneTrack));
		}

		pSceneDatabase->AddAnimation(cd::MoveTemp(animation));
	}
}
	
}
