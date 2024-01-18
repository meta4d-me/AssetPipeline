#include "FbxProducerImpl.h"

#include "Hashers/StringHash.hpp"
#include "Scene/SceneDatabase.h"

#include <fbxsdk.h>

#include <cassert>
#include <format>
#include <optional>
#include <vector>

namespace
{

void PrintLog(const std::string& msg)
{
	printf("[AssetPipeline][FbxProducer] %s\n", msg.c_str());
}

}

namespace details
{

cd::Transform ConvertFbxNodeTransform(fbxsdk::FbxNode* pNode)
{
	fbxsdk::FbxDouble3 translation = pNode->LclTranslation.EvaluateValue(FBXSDK_TIME_ZERO);
	fbxsdk::FbxDouble3 rotation = pNode->LclRotation.EvaluateValue(FBXSDK_TIME_ZERO);
	fbxsdk::FbxDouble3 scaling = pNode->LclScaling.EvaluateValue(FBXSDK_TIME_ZERO);
	return cd::Transform(
		cd::Vec3f(static_cast<float>(translation[0]), static_cast<float>(translation[1]), static_cast<float>(translation[2])),
		cd::Quaternion::FromPitchYawRoll(static_cast<float>(rotation[1]), static_cast<float>(rotation[2]), static_cast<float>(rotation[0])),
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

cd::Vec3f ConvertFbxPosition(const fbxsdk::FbxVector4& position)
{
	return cd::Vec3f(static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]));
}

cd::Quaternion ConvertFbxRotation(const fbxsdk::FbxQuaternion& rotation)
{
	return cd::Quaternion(static_cast<float>(rotation[0]), static_cast<float>(rotation[1]), static_cast<float>(rotation[2]), static_cast<float>(rotation[3]));
}

cd::Vec3f ConvertFbxScale(const fbxsdk::FbxVector4& scale)
{
	return cd::Vec3f(static_cast<float>(scale[0]), static_cast<float>(scale[1]), static_cast<float>(scale[2]));
}

cd::Transform ConvertFbxTransform(const fbxsdk::FbxVector4& position, const fbxsdk::FbxQuaternion& rotation, const fbxsdk::FbxVector4& scale)
{
	return cd::Transform(ConvertFbxPosition(position), ConvertFbxRotation(rotation), ConvertFbxScale(scale));
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

fbxsdk::FbxAMatrix CastFbxMatrixToFbxAffineMatrix(const fbxsdk::FbxMatrix& matrix)
{
	return *(fbxsdk::FbxAMatrix*)(double*)&matrix;
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

fbxsdk::FbxNode* GetRootBoneNode(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode)
{
	fbxsdk::FbxNode* pRootBoneNode = pNode;
	fbxsdk::FbxNode* pSceneRoot = pScene->GetRootNode();

	while (pRootBoneNode && pRootBoneNode->GetParent())
	{
		fbxsdk::FbxNodeAttribute* pNodeAttribute = pRootBoneNode->GetParent()->GetNodeAttribute();
		if (pSceneRoot == pRootBoneNode->GetParent() || !pNodeAttribute)
		{
			break;
		}

		auto attributeType = pNodeAttribute->GetAttributeType();
		bool isExpectedNode = fbxsdk::FbxNodeAttribute::eMesh == attributeType ||
			fbxsdk::FbxNodeAttribute::eNull == attributeType ||
			fbxsdk::FbxNodeAttribute::eSkeleton == attributeType;
		if (!isExpectedNode)
		{
			break;
		}

		if (fbxsdk::FbxNodeAttribute::eMesh == attributeType)
		{
			// In special case, skeletal mesh can be ancestor of bones.
			fbxsdk::FbxMesh* pMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pNodeAttribute);
			if (pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin) > 0)
			{
				break;
			}

			pRootBoneNode = pRootBoneNode->GetParent();
		}
		else if (fbxsdk::FbxNodeAttribute::eNull == attributeType ||
			fbxsdk::FbxNodeAttribute::eSkeleton == attributeType)
		{
			pRootBoneNode = pRootBoneNode->GetParent();
		}
		else
		{
			break;
		}
	}

	return pRootBoneNode;
}

void FindFbxSkeletalMeshRecursively(fbxsdk::FbxScene* pScene, fbxsdk::FbxNode* pNode, std::vector<std::vector<fbxsdk::FbxNode*>>& skeletalMeshArrays, std::vector<fbxsdk::FbxNode*>& skeletons)
{
	fbxsdk::FbxNode* pSkeletalMeshNode = nullptr;
	fbxsdk::FbxNode* pNodeToAdd = pNode;
	fbxsdk::FbxVector4 noScale(1.0, 1.0, 1.0);

	if (pNode->GetMesh() && pNode->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		pSkeletalMeshNode = pNode;
	}

	if (pSkeletalMeshNode)
	{
		fbxsdk::FbxMesh* pMesh = pSkeletalMeshNode->GetMesh();
		assert(pMesh);

		if (fbxsdk::FbxSkin* pSkinDeformer = static_cast<fbxsdk::FbxSkin*>(pMesh->GetDeformer(0, fbxsdk::FbxDeformer::eSkin)))
		{
			bool hasWeightData = false;

			uint32_t clusterCount = pSkinDeformer->GetClusterCount();
			for (uint32_t clusterIndex = 0U; clusterIndex < clusterCount; ++clusterIndex)
			{
				fbxsdk::FbxNode* pRootBone = pSkinDeformer->GetCluster(clusterIndex)->GetLink();
				pRootBone = GetRootBoneNode(pScene, pRootBone);
				if (!pRootBone)
				{
					continue;
				}

				bool addToExistedSkeleton = false;
				for (std::size_t skeletonIndex = 0U; skeletonIndex < skeletons.size(); ++skeletonIndex)
				{
					fbxsdk::FbxNode* pSkeleton = skeletons[skeletonIndex];
					if (pRootBone == pSkeleton)
					{
						skeletalMeshArrays[skeletonIndex].emplace_back(pNodeToAdd);
						addToExistedSkeleton = true;
						break;
					}
				}

				if (!addToExistedSkeleton)
				{
					std::vector<fbxsdk::FbxNode*> skeletalMeshArray;
					skeletalMeshArray.push_back(pNodeToAdd);
					skeletalMeshArrays.emplace_back(cd::MoveTemp(skeletalMeshArray));
					skeletons.push_back(pRootBone);

					if (pNodeToAdd->EvaluateLocalScaling() != noScale)
					{
						PrintLog(std::format("Error : Skeletal mesh {} has non-identity scale which may cause wrong results.", pNodeToAdd->GetName()));
					}
				}

				hasWeightData = true;
				break;
			}

			if (!hasWeightData)
			{
				PrintLog(std::format("Warning : Ignored skeletal mesh {} because there are no weights data.", pNodeToAdd->GetName()));
			}
		}
	}

	std::vector<fbxsdk::FbxNode*> scaledChildNodes;
	for (int32_t childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		auto* pChildNode = pNode->GetChild(childIndex);
		if (pChildNode->EvaluateLocalScaling() != noScale)
		{
			scaledChildNodes.push_back(pChildNode);
		}
		else
		{
			FindFbxSkeletalMeshRecursively(pScene, pChildNode, skeletalMeshArrays, skeletons);
		}
	}

	for (auto* scaledChildNode : scaledChildNodes)
	{
		FindFbxSkeletalMeshRecursively(pScene, scaledChildNode, skeletalMeshArrays, skeletons);
	}
}

void FixSkeletonRecursively(fbxsdk::FbxManager* pManager, fbxsdk::FbxNode* pNode, std::vector<fbxsdk::FbxNode*> skeletalMeshes)
{
	auto* pNodeAttribute = pNode->GetNodeAttribute();
	bool isLODGroupNode = pNodeAttribute && fbxsdk::FbxNodeAttribute::eLODGroup == pNodeAttribute->GetAttributeType();
	if (!isLODGroupNode)
	{
		for (int32_t childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
		{
			FixSkeletonRecursively(pManager, pNode->GetChild(childIndex), skeletalMeshes);
		}
	}

	if (!pNodeAttribute)
	{
		return;
	}

	bool isValidMeshNode = true;
	if (fbxsdk::FbxNodeAttribute::eMesh == pNodeAttribute->GetAttributeType())
	{
		int32_t childCount = pNode->GetChildCount();
		int32_t childIndex;
		for (childIndex = 0; childIndex < childCount; ++childIndex)
		{
			auto* pChildNode = pNode->GetChild(childIndex);
			if (!pNode->GetMesh())
			{
				break;
			}
		}

		isValidMeshNode = childIndex == childCount;
	}
	else if (fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
	{
		isValidMeshNode = false;
	}

	if (isValidMeshNode)
	{
		if (std::find(skeletalMeshes.begin(), skeletalMeshes.end(), pNode) != skeletalMeshes.end())
		{
			skeletalMeshes.push_back(pNode);
		}
	}
	else
	{
		skeletalMeshes.erase(std::remove(skeletalMeshes.begin(), skeletalMeshes.end(), pNode), skeletalMeshes.end());

		// Replace with skeleton
		auto* pSkeleton = fbxsdk::FbxSkeleton::Create(pManager, "");
		pNode->SetNodeAttribute(pSkeleton);
		pSkeleton->SetSkeletonType(FbxSkeleton::eLimbNode);
	}
}

void RetrievePoseFromBindPose(fbxsdk::FbxScene* pScene, const std::vector<fbxsdk::FbxNode*>& skeletalMeshNodes, std::vector<fbxsdk::FbxPose*>& poses)
{
	int32_t scenePoseCount = pScene->GetPoseCount();
	for (int32_t poseIndex = 0; poseIndex < scenePoseCount; ++poseIndex)
	{
		fbxsdk::FbxPose* pPose = pScene->GetPose(poseIndex);
		if (!pPose->IsBindPose())
		{
			continue;
		}

		for (auto* skeletalMeshNode : skeletalMeshNodes)
		{
			fbxsdk::NodeList missingAncestors, missingDeformers, missingDeformersAncestors, wrongMatrices;
			if (pPose->IsValidBindPoseVerbose(skeletalMeshNode, missingAncestors, missingDeformers, missingDeformersAncestors, wrongMatrices))
			{
				poses.push_back(pPose);
				break;
			}
			else
			{
				for (int32_t ancestorIndex = 0; ancestorIndex < missingAncestors.GetCount(); ++ancestorIndex)
				{
					fbxsdk::FbxNode* pAncestor = missingAncestors.GetAt(ancestorIndex);
					pPose->Add(pAncestor, pAncestor->EvaluateGlobalTransform(FBXSDK_TIME_ZERO));
				}

				if (pPose->IsValidBindPose(skeletalMeshNode))
				{
					poses.push_back(pPose);
					break;
				}
				else
				{
					fbxsdk::FbxNode* pParent = skeletalMeshNode->GetParent();
					while (pParent)
					{
						fbxsdk::FbxNodeAttribute* pAttribute = pParent->GetNodeAttribute();
						if (pAttribute && FbxNodeAttribute::eNull == pAttribute->GetAttributeType())
						{
							break;
						}

						pParent = pParent->GetParent();
					}

					if (pParent && pPose->IsValidBindPose(pParent))
					{
						poses.push_back(pPose);
						break;
					}
					else
					{
						PrintLog(std::format("Warning : failed to fix bind pose {} for skeletal mesh node {}", pPose->GetName(), skeletalMeshNode->GetName()));
					}
				}
			}
		}
	}
}

void BuildSkeletonRecursively(fbxsdk::FbxNode* pLink, std::vector<fbxsdk::FbxNode*>& linkBones)
{
	fbxsdk::FbxNodeAttribute* pAttribute = pLink->GetNodeAttribute();
	if (!pAttribute)
	{
		return;
	}

	fbxsdk::FbxNodeAttribute::EType attributeType = pAttribute->GetAttributeType();
	if (fbxsdk::FbxNodeAttribute::eSkeleton == attributeType ||
		fbxsdk::FbxNodeAttribute::eMesh == attributeType)
	{
		linkBones.push_back(pLink);

		for (int32_t childIndex = 0; childIndex < pLink->GetChildCount(); ++childIndex)
		{
			BuildSkeletonRecursively(pLink->GetChild(childIndex), linkBones);
		}
	}
	else if (fbxsdk::FbxNodeAttribute::eNull == attributeType)
	{
		linkBones.push_back(pLink);
	}
}

void BuildSkeletonSystem(fbxsdk::FbxScene* pScene, std::vector<fbxsdk::FbxCluster*>& clusters, std::vector<fbxsdk::FbxNode*>& linkBones)
{
	fbxsdk::FbxNode* pLinkBone;
	std::vector<fbxsdk::FbxNode*> rootLinkBones;

	size_t clusterCount = clusters.size();
	for (size_t clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
	{
		fbxsdk::FbxCluster* pCluster = clusters[clusterIndex];
		pLinkBone = pCluster->GetLink();
		if (pLinkBone)
		{
			pLinkBone = GetRootBoneNode(pScene, pLinkBone);
			size_t linkIndex;
			for (linkIndex = 0; linkIndex < rootLinkBones.size(); ++linkIndex)
			{
				if (pLinkBone == rootLinkBones[linkIndex])
				{
					break;
				}
			}

			if (linkIndex == rootLinkBones.size())
			{
				rootLinkBones.push_back(pLinkBone);
			}
		}
	}

	for (size_t rootLinkIndex = 0; rootLinkIndex < rootLinkBones.size(); ++rootLinkIndex)
	{
		BuildSkeletonRecursively(rootLinkBones[rootLinkIndex], linkBones);
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

	// Default options.
	m_options.Enable(FbxProducerOptions::ImportStaticMesh);
	m_options.Enable(FbxProducerOptions::ImportMaterial);
	m_options.Enable(FbxProducerOptions::ImportTexture);
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
	pSceneDatabase->SetName(m_filePath.c_str());

	fbxsdk::FbxIOSettings* pIOSettings = m_pSDKManager->GetIOSettings();

	// Query SDK version and initialize Importer and IOSettings for opening file.
	int32_t sdkMajorVersion = 0, sdkMinorVersion = 0, sdkRevision = 0;
	fbxsdk::FbxManager::GetFileFormatVersion(sdkMajorVersion, sdkMinorVersion, sdkRevision);
	PrintLog(std::format("FBXSDK Version : {}, {}, {}", sdkMajorVersion, sdkMinorVersion, sdkRevision));

	fbxsdk::FbxImporter* pSDKImporter = fbxsdk::FbxImporter::Create(m_pSDKManager, "FbxProducer");
	assert(pSDKImporter && "Failed to init sdk importer.");
	pIOSettings->SetBoolProp(IMP_RELAXED_FBX_CHECK, true);
	if (!pSDKImporter->Initialize(m_filePath.c_str(), -1, pIOSettings))
	{
		if (fbxsdk::FbxStatus::eInvalidFileVersion == pSDKImporter->GetStatus().GetCode())
		{
			PrintLog("Error : Failed to import because the fbx file version is invalid.\n");
			return;
		}
	}

	// Query fbx file version and import file to scene.
	int32_t fileMajorVersion = 0, fileMinorVersion = 0, fileRevision = 0;
	pSDKImporter->GetFileVersion(fileMajorVersion, fileMinorVersion, fileRevision);
	PrintLog(std::format("FBXFile Version : {}, {}, {}", fileMajorVersion, fileMinorVersion, fileRevision));

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

	// Build scene information :
	// 1.Preprocess fbx scene to get what we expect.
	// 2.Fix scene nodes which will pop up warnings about not very correct result.
	// 3.Block bad scene nodes which will pop up errors about wrong results.
	FbxSceneInfo sceneInfo;
	BuildSceneInfo(pSDKScene, sceneInfo);

	if (IsOptionEnabled(FbxProducerOptions::ImportMaterial))
	{
		// TODO : ImportTexture logic is nested in ImportMaterial. If you want to only import textures, need to refact.
		for (const auto& surfaceMaterial : sceneInfo.surfaceMaterials)
		{
			ImportMaterial(surfaceMaterial, pSceneDatabase);
		}
	}

	{
		// Convert transform hierarchy nodes.
		for (const auto& transformNode : sceneInfo.transformNodes)
		{
			ImportNode(transformNode, pSceneDatabase);
		}

		// Add child nodes to parent nodes.
		// In a standalone pass because it doesn't to depend on node creating orders.
		for (const auto& node : pSceneDatabase->GetNodes())
		{
			if (node.GetParentID().IsValid())
			{
				pSceneDatabase->GetNode(node.GetParentID().Data()).AddChildID(node.GetID());
			}
			else
			{
				pSceneDatabase->AddRootNodeID(node.GetID());
			}
		}
	}

	if (IsOptionEnabled(FbxProducerOptions::ImportStaticMesh))
	{
		for (const auto& staticMesh : sceneInfo.staticMeshes)
		{
			// In fbx scene graph, a node has a container named node attribute which is used to store mesh/light/...
			cd::NodeID nodeID = ImportNode(staticMesh, pSceneDatabase);
			cd::MeshID meshID = ImportMesh(staticMesh->GetMesh(), pSceneDatabase);

			cd::Node& node = pSceneDatabase->GetNode(nodeID.Data());
			node.AddMeshID(meshID);

			if (fbxsdk::FbxNode* pParentNode = staticMesh->GetParent())
			{
				cd::NodeID parentNodeID = AllocateNodeID(pParentNode);
				node.SetParentID(parentNodeID);
				pSceneDatabase->GetNode(parentNodeID.Data()).AddChildID(nodeID);
			}

			AssociateMeshWithBlendShape(staticMesh->GetMesh(), meshID, pSceneDatabase);
		}
	}
	
	if (IsOptionEnabled(FbxProducerOptions::ImportLight))
	{
		for (const auto& light : sceneInfo.lights)
		{
			// TODO : support node hierarchy. Currently, it is always in global world space.
			ImportLight(light, pSceneDatabase);
		}
	}

	if (IsOptionEnabled(FbxProducerOptions::ImportSkeleton))
	{
		for (const auto& skeletalMeshArray : sceneInfo.skeletalMeshArrays)
		{
			ImportSkeletonBones(pSDKScene, skeletalMeshArray, pSceneDatabase);
		}
	}

	if (IsOptionEnabled(FbxProducerOptions::ImportSkeletalMesh))
	{
		for (const auto& skeletalMeshArray : sceneInfo.skeletalMeshArrays)
		{
			for (const auto& skeletalMesh : skeletalMeshArray)
			{
				ImportSkeletalMesh(skeletalMesh->GetMesh(), pSceneDatabase);
			}
		}
	}

	if (IsOptionEnabled(FbxProducerOptions::ImportAnimation))
	{
		if (auto* pAnimStack = pSDKScene->GetSrcObject<fbxsdk::FbxAnimStack>(0))
		{
			pSDKScene->SetCurrentAnimationStack(pAnimStack);
			ImportAnimation(pSDKScene, pSceneDatabase);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene
//////////////////////////////////////////////////////////////////////////////////////////////////////
void FbxProducerImpl::BuildSceneInfo(fbxsdk::FbxScene* pScene, FbxSceneInfo& sceneInfo)
{
	sceneInfo.rootNode = pScene->GetRootNode();
	
	// Query scene nodes except SkeletalMesh, SkeletonBones.
	TraverseSceneRecursively(sceneInfo.rootNode, sceneInfo);

	if (IsOptionEnabled(FbxProducerOptions::ImportSkeleton) || IsOptionEnabled(FbxProducerOptions::ImportSkeletalMesh))
	{
		// Query SkeletalMesh, SkeletonBones.
		// TODO : combine these logics to TraverseSceneRecursively. Currently, it refers to Unreal Engine's skeletal mesh pipeline processing.
		details::FindFbxSkeletalMeshRecursively(pScene, sceneInfo.rootNode, sceneInfo.skeletalMeshArrays, sceneInfo.skeletonRootBones);

		// Fix invalid bind pose for skeleton.
		std::size_t skeletonCount = sceneInfo.skeletonRootBones.size();
		for (std::size_t skeletonIndex = 0; skeletonIndex < skeletonCount; ++skeletonIndex)
		{
			details::FixSkeletonRecursively(m_pSDKManager, sceneInfo.skeletonRootBones[skeletonIndex], sceneInfo.skeletalMeshArrays[skeletonIndex]);
		}
	}

	if (IsOptionEnabled(FbxProducerOptions::ImportMaterial))
	{
		fbxsdk::FbxArray<fbxsdk::FbxSurfaceMaterial*> sdkMaterials;
		pScene->FillMaterialArray(sdkMaterials);
		for (int32_t materialIndex = 0; materialIndex < sdkMaterials.Size(); ++materialIndex)
		{
			fbxsdk::FbxSurfaceMaterial* pSDKMaterial = sdkMaterials[materialIndex];
			sceneInfo.surfaceMaterials.push_back(pSDKMaterial);
		}
	}
}

void FbxProducerImpl::TraverseSceneRecursively(fbxsdk::FbxNode* pNode, FbxSceneInfo& sceneInfo)
{
	fbxsdk::FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (nullptr == pNodeAttribute || fbxsdk::FbxNodeAttribute::eNull == pNodeAttribute->GetAttributeType())
	{
		sceneInfo.transformNodes.push_back(pNode);
	}
	else if (fbxsdk::FbxNodeAttribute::eLight == pNodeAttribute->GetAttributeType())
	{
		auto* pFbxLight = reinterpret_cast<fbxsdk::FbxLight*>(pNodeAttribute);
		assert(pFbxLight);
		sceneInfo.lights.push_back(pFbxLight);
	}
	else if (fbxsdk::FbxNodeAttribute::eMesh == pNodeAttribute->GetAttributeType())
	{
		fbxsdk::FbxMesh* pMesh = pNode->GetMesh();
		if (!pMesh->IsTriangleMesh() && IsOptionEnabled(FbxProducerOptions::Triangulate))
		{
			const bool bReplace = true;
			fbxsdk::FbxNodeAttribute* pConvertedNode = m_pSDKGeometryConverter->Triangulate(pMesh, bReplace);
			if (pConvertedNode && pConvertedNode->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				pMesh = reinterpret_cast<fbxsdk::FbxMesh*>(pConvertedNode);
				assert(pMesh && pMesh->IsTriangleMesh());
			}
			else
			{
				pMesh = nullptr;
				PrintLog("Error : failed to triangulate mesh.");
			}
		}

		if (pMesh)
		{
			int32_t skinCount = pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
			if (skinCount == 0)
			{
				sceneInfo.staticMeshes.push_back(pNode);
			}
		}
	}

	for (int32_t childIndex = 0; childIndex < pNode->GetChildCount(); ++childIndex)
	{
		TraverseSceneRecursively(pNode->GetChild(childIndex), sceneInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Transform node
//////////////////////////////////////////////////////////////////////////////////////////////////////
cd::NodeID FbxProducerImpl::AllocateNodeID(const fbxsdk::FbxNode* pSDKNode)
{
	return m_nodeIDGenerator.AllocateID(static_cast<uint32_t>(pSDKNode->GetUniqueID()));
}

cd::NodeID FbxProducerImpl::ImportNode(const fbxsdk::FbxNode* pSDKNode, cd::SceneDatabase* pSceneDatabase)
{
	cd::NodeID nodeID = AllocateNodeID(pSDKNode);
	cd::Node node(nodeID, pSDKNode->GetName());
	node.SetTransform(details::ConvertFbxNodeTransform(const_cast<fbxsdk::FbxNode*>(pSDKNode)));

	if (auto* pParentNode = pSDKNode->GetParent())
	{
		cd::NodeID parentNodeID = AllocateNodeID(pParentNode);
		node.SetParentID(parentNodeID);
	}

	pSceneDatabase->AddNode(cd::MoveTemp(node));

	return nodeID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Light
//////////////////////////////////////////////////////////////////////////////////////////////////////
cd::LightID FbxProducerImpl::ImportLight(const fbxsdk::FbxLight* pFbxLight, cd::SceneDatabase* pSceneDatabase)
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
	cd::Transform transform = details::ConvertFbxNodeTransform(pFbxLight->GetNode());

	cd::Light light(lightID, lightType);
	light.SetName(pFbxLight->GetName());
	light.SetColor(cd::Vec3f(lightColor[0], lightColor[1], lightColor[2]));
	light.SetIntensity(lightIntensity);

	auto [angleScale, angleOffset] = light.CalculateScaleAndOffset(static_cast<float>(pFbxLight->InnerAngle.Get()), static_cast<float>(pFbxLight->OuterAngle.Get()));
	light.SetAngleScale(angleScale);
	light.SetAngleOffset(angleOffset);

	light.SetPosition(transform.GetTranslation());
	light.SetUp(transform.GetRotation() * cd::Vec3f(0.0f, 1.0f, 0.0f));
	light.SetDirection(transform.GetRotation() * cd::Vec3f(1.0f, 0.0f, 0.0f));

	pSceneDatabase->AddLight(cd::MoveTemp(light));

	return lightID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Material
//////////////////////////////////////////////////////////////////////////////////////////////////////
std::pair<cd::MaterialID, bool> FbxProducerImpl::AllocateMaterialID(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial)
{
	uint32_t materialHash = cd::StringHash<cd::MaterialID::ValueType>(pSDKMaterial->GetName());
	bool isReused;
	cd::MaterialID materialID = m_materialIDGenerator.AllocateID(materialHash, &isReused);
	return std::make_pair(materialID, isReused);
}

void FbxProducerImpl::ImportMaterialProperty(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, const char* pPropertyName, cd::Material* pMaterial)
{
	// TODO
	pSDKMaterial;
	pPropertyName;
	pMaterial;
}

void FbxProducerImpl::ImportMaterialTexture(const fbxsdk::FbxProperty& sdkProperty, cd::MaterialTextureType textureType, cd::Material& material, cd::SceneDatabase* pSceneDatabase)
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

cd::MaterialID FbxProducerImpl::ImportMaterial(const fbxsdk::FbxSurfaceMaterial* pSDKMaterial, cd::SceneDatabase* pSceneDatabase)
{
	auto [materialID, isReused] = AllocateMaterialID(pSDKMaterial);
	if (isReused)
	{
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
				ImportMaterialTexture(currentProperty, itPropertyName->second, material, pSceneDatabase);
			}

			currentProperty = pSDKMaterial->GetNextProperty(currentProperty);
		}
	}

	pSceneDatabase->AddMaterial(cd::MoveTemp(material));

	return materialID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh
//////////////////////////////////////////////////////////////////////////////////////////////////////
cd::MeshID FbxProducerImpl::ImportMesh(const fbxsdk::FbxMesh* pFbxMesh, cd::SceneDatabase* pSceneDatabase)
{
	cd::MeshID meshID(m_meshIDGenerator.AllocateID());
	const fbxsdk::FbxNode* pSDKNode = pFbxMesh->GetNode();

	cd::Mesh mesh;
	mesh.SetID(meshID);
	mesh.SetName(pSDKNode->GetName());

	uint32_t materialCount = pSDKNode->GetMaterialCount();
	bool isMaterialEmpty = 0U == materialCount;

	// No mesh layer or multiple layers to parse?
	// Currently, we only parse the first layer.
	// assert(pFbxMesh->GetLayerCount() != 1);
	const fbxsdk::FbxLayer* pMeshBaseLayer = pFbxMesh->GetLayer(0);

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
	const fbxsdk::FbxLayerElementMaterial* pLayerElementMaterial = pMeshBaseLayer->GetMaterials();
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

	// If smoothing group exists, convert to edge mapping mode.
	// SoftEdge : one vertex position has only one normal which doesn't need to split.
	// HardEdge : one vertex position has multiple normals which needs to split.
	//fbxsdk::FbxLayerElementSmoothing* pSmoothing = pMeshBaseLayer->GetSmoothing();
	//fbxsdk::FbxLayerElement::EMappingMode smoothingMappingMode = pSmoothing ? pSmoothing->GetMappingMode() : fbxsdk::FbxLayerElement::EMappingMode::eNone;
	//fbxsdk::FbxLayerElement::EReferenceMode smoothingReferenceMode = pSmoothing ? pSmoothing->GetReferenceMode() : fbxsdk::FbxLayerElement::EReferenceMode::eDirect;
	//if (fbxsdk::FbxLayerElement::EMappingMode::eByPolygon == smoothingMappingMode)
	//{
	//	m_pSDKGeometryConverter->ComputeEdgeSmoothingFromPolygonSmoothing(pFbxMesh);
	//	pSmoothing = pMeshBaseLayer->GetSmoothing();
	//	smoothingMappingMode = pSmoothing->GetMappingMode();
	//	smoothingReferenceMode = pSmoothing->GetReferenceMode();
	//}
	//assert(fbxsdk::FbxLayerElement::EMappingMode::eByEdge == smoothingMappingMode || fbxsdk::FbxLayerElement::EMappingMode::eNone == smoothingMappingMode);

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
			uint32_t vertexInstanceID = polygonVertexBeginIndex + polygonVertexIndex;
			mesh.SetVertexIDToInstance(controlPointIndex, vertexInstanceID);
			mesh.SetVertexInstanceToID(vertexInstanceID, controlPointIndex);

			polygon.push_back(vertexInstanceID);
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

	pSceneDatabase->AddMesh(cd::MoveTemp(mesh));

	return meshID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Skeletal Mesh
//////////////////////////////////////////////////////////////////////////////////////////////////////
void FbxProducerImpl::ImportSkeletonBones(fbxsdk::FbxScene* pScene, const std::vector<fbxsdk::FbxNode*>& skeletalMeshNodes, cd::SceneDatabase* pSceneDatabase)
{
	// Collect all skin clusters.
	std::vector<fbxsdk::FbxCluster*> skinClusters;
	for (auto* skeletalMeshNode : skeletalMeshNodes)
	{
		fbxsdk::FbxMesh* pMesh = skeletalMeshNode->GetMesh();
		int32_t skinDeformerCount = pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
		for (int32_t skinDeformerIndex = 0; skinDeformerIndex < skinDeformerCount; ++skinDeformerIndex)
		{
			auto* pSkin = static_cast<fbxsdk::FbxSkin*>(pMesh->GetDeformer(skinDeformerIndex, fbxsdk::FbxDeformer::eSkin));
			fbxsdk::FbxSkin::EType skinningType = pSkin->GetSkinningType();
			if (skinningType == fbxsdk::FbxSkin::eRigid || skinningType == fbxsdk::FbxSkin::eLinear)
			{
				int32_t skinClusterCount = pSkin->GetClusterCount();
				for (int32_t skinClusterIndex = 0; skinClusterIndex < skinClusterCount; ++skinClusterIndex)
				{
					fbxsdk::FbxCluster* pCluster = pSkin->GetCluster(skinClusterIndex);
					if (pCluster->GetLinkMode() != fbxsdk::FbxCluster::eNormalize)
					{
						PrintLog("Error : unsupported skin cluster link mode.");
						return;
					}

					skinClusters.push_back(pCluster);
				}
			}
			else
			{
				PrintLog("Error : unsupported skinning type.");
				return;
			}
		}
	}

	if (skinClusters.empty())
	{
		PrintLog("Error : no associated skin clusters.");
		return;
	}

	// Collect all bind poses.
	// If we can't find valid bind poses, try to recreate them. 
	std::vector<fbxsdk::FbxPose*> poses;
	details::RetrievePoseFromBindPose(pScene, skeletalMeshNodes, poses);
	if (poses.empty())
	{
		PrintLog("Warning : recreating bind poses.");
		for (int32_t poseIndex = pScene->GetPoseCount() - 1; poseIndex >= 0; --poseIndex)
		{
			fbxsdk::FbxPose* pPose = pScene->GetPose(poseIndex);
			if (pPose->IsBindPose())
			{
				pScene->RemovePose(poseIndex);
				pPose->Destroy();
			}
		}

		m_pSDKManager->CreateMissingBindPoses(pScene);
		details::RetrievePoseFromBindPose(pScene, skeletalMeshNodes, poses);
		if (poses.empty())
		{
			PrintLog("Error : failed to recreate bind poses.");
		}
		else
		{
			PrintLog("Info : succeed to recreate bind poses.");
		}
	}

	if (poses.empty())
	{
		PrintLog("Error : can't find valid poses. TODO : support using time0 pose as refernce.");
		return;
	}

	// Collect skeleton bone chains.
	std::vector<fbxsdk::FbxNode*> linkBones;
	details::BuildSkeletonSystem(pScene, skinClusters, linkBones);

	if (linkBones.empty())
	{
		PrintLog("Error : can't find valid bones.");
		return;
	}

	// Validate bone chain. Here are invalid or unsupported cases.
	// 1. Multiple root bones.
	// 2. Bones reference to different skeleton nodes.
	std::vector<size_t> rootBoneIndexes;
	size_t linkBoneCount = linkBones.size();
	for (size_t boneIndex = 0; boneIndex < linkBoneCount; ++boneIndex)
	{
		fbxsdk::FbxNode* pLinkBone = linkBones[boneIndex];
		fbxsdk::FbxNode* pLinkParentBone = pLinkBone->GetParent();
		if (auto itParentIndex = std::find(linkBones.begin(), linkBones.end(), pLinkParentBone); itParentIndex == linkBones.end())
		{
			rootBoneIndexes.push_back(boneIndex);
		}
	}

	if (rootBoneIndexes.size() > 1)
	{
		PrintLog("Error : can't find parent bone. The file may contains multiple root bones?");
		return;
	}

	// Create cd::Skeleton.
	cd::Skeleton skeleton;
	skeleton.SetID(m_skeletonIDGenerator.AllocateID());

	// Create cd::Bones.
	std::vector<cd::Bone> bones;
	bones.resize(linkBoneCount);
	for (size_t boneIndex = 0; boneIndex < linkBoneCount; ++boneIndex)
	{
		fbxsdk::FbxNode* pLinkBone = linkBones[boneIndex];

		cd::Bone& cdBone = bones[boneIndex];
		cdBone.SetID(m_boneIDGenerator.AllocateID());
		cdBone.SetName(pLinkBone->GetName());
		cdBone.SetSkeletonID(skeleton.GetID());
		skeleton.AddBoneID(cdBone.GetID());

		// LibmLength/Size is used for editor/debug draw.
		fbxsdk::FbxSkeleton* pSkeleton = pLinkBone->GetSkeleton();
		if (pSkeleton)
		{
			cdBone.SetLimbLength(static_cast<float>(pSkeleton->LimbLength.Get()));
			cdBone.SetLimbSize(cd::Vec3f(static_cast<float>(pSkeleton->Size.Get())));
		}
		else
		{
			cdBone.SetLimbLength(1.0f);
			cdBone.SetLimbSize(cd::Vec3f(100.0f));
		}
	}

	// Associate with each other by ID.
	size_t rootBoneIndex = rootBoneIndexes[0];
	skeleton.SetRootBoneID(bones[rootBoneIndex].GetID());
	for (size_t boneIndex = 0; boneIndex < linkBoneCount; ++boneIndex)
	{
		cd::Bone& cdBone = bones[boneIndex];
		if (boneIndex != rootBoneIndex)
		{
			fbxsdk::FbxNode* pLinkBone = linkBones[boneIndex];
			fbxsdk::FbxNode* pLinkParentBone = pLinkBone->GetParent();

			auto itParentIndex = std::find(linkBones.begin(), linkBones.end(), pLinkParentBone);
			assert(itParentIndex != linkBones.end());

			cd::Bone& parentBone = bones[itParentIndex - linkBones.begin()];
			cdBone.SetParentID(parentBone.GetID());
			parentBone.AddChildID(cdBone.GetID());
		}
	}

	// Calculate transform matrixs from BindPose/SkinCluster/Bone/...
	// TODO : 1.use time0 pose 2.apply axis system transform.
	// TODO : check if transform matrix contains invalid data : nan, zero scale, .. 
	// TODO : build matrix by applying import option's T/R/S.
	std::vector<fbxsdk::FbxAMatrix> globalTransformPerLinkBone;
	globalTransformPerLinkBone.resize(bones.size());
	globalTransformPerLinkBone[skeleton.GetRootBoneID().Data()].SetIdentity();
	
	for (size_t boneIndex = 0; boneIndex < linkBoneCount; ++boneIndex)
	{
		cd::Bone& cdBone = bones[boneIndex];
		uint32_t cdBoneID = cdBone.GetID().Data();
		uint32_t cdBoneParentID = cdBone.GetParentID().Data();

		fbxsdk::FbxNode* pLinkBone = linkBones[boneIndex];
		
		bool linkBoneFindTransform = false;

		// Link bones may not have clusters, but get global transform matrix from bind pose.
		for (const auto& pose : poses)
		{
			int32_t poseLinkIndex = pose->Find(pLinkBone);
			if (poseLinkIndex != -1)
			{
				assert(poseLinkIndex >= 0);
				globalTransformPerLinkBone[cdBoneID] = details::CastFbxMatrixToFbxAffineMatrix(pose->GetMatrix(poseLinkIndex));
				linkBoneFindTransform = true;
				break;
			}
		}

		// Then search for skin clusters to get global transform matrix.
		if (!linkBoneFindTransform)
		{
			for (const auto& skinCluster : skinClusters)
			{
				if (skinCluster->GetLink() == pLinkBone)
				{
					skinCluster->GetTransformLinkMatrix(globalTransformPerLinkBone[cdBoneID]);
					linkBoneFindTransform = true;
					break;
				}
			}
		}

		// Still not found, get global transform from bone node itself.
		if (!linkBoneFindTransform)
		{
			globalTransformPerLinkBone[cdBoneID] = pLinkBone->EvaluateGlobalTransform();
		}

		// Get local transform materix for link bone.
		fbxsdk::FbxVector4 localLinkT;
		fbxsdk::FbxQuaternion localLinkQ;
		fbxsdk::FbxVector4 localLinkS;
		if (boneIndex != rootBoneIndex)
		{
			fbxsdk::FbxAMatrix transform = globalTransformPerLinkBone[cdBoneParentID].Inverse() * globalTransformPerLinkBone[cdBoneID];
			localLinkT = transform.GetT();
			localLinkQ = transform.GetQ();
			localLinkS = transform.GetS();
		}
		else
		{
			// RootBone is already in global coordinate system.
			const auto& rootTransform = globalTransformPerLinkBone[cdBoneID];
			localLinkT = rootTransform.GetT();
			localLinkQ = rootTransform.GetQ();
			localLinkS = rootTransform.GetS();
		}

		cdBone.SetTransform(details::ConvertFbxTransform(localLinkT, localLinkQ, localLinkS));
	}

	// Add to scene database.
	pSceneDatabase->AddSkeleton(cd::MoveTemp(skeleton));
	for (auto& bone : bones)
	{
		pSceneDatabase->AddBone(cd::MoveTemp(bone));
	}
}

cd::MeshID FbxProducerImpl::ImportSkeletalMesh(const fbxsdk::FbxMesh* pFbxMesh, cd::SceneDatabase* pSceneDatabase)
{
	cd::MeshID meshID = ImportMesh(pFbxMesh, pSceneDatabase);
	auto& mesh = pSceneDatabase->GetMesh(meshID.Data());
	int32_t skinDeformerCount = pFbxMesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
	for (int32_t skinIndex = 0; skinIndex < skinDeformerCount; ++skinIndex)
	{
		const auto* pSkin = static_cast<fbxsdk::FbxSkin*>(pFbxMesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin));
		auto skinID = ImportSkin(pSkin, mesh, pSceneDatabase);
		if (skinID.IsValid())
		{
			mesh.AddSkinID(skinID);
		}
	}

	return meshID;
}

cd::SkinID FbxProducerImpl::ImportSkin(const fbxsdk::FbxSkin* pSkin, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase)
{
	assert(pSkin);
	assert(fbxsdk::FbxSkin::eLinear == pSkin->GetSkinningType() || fbxsdk::FbxSkin::eRigid == pSkin->GetSkinningType());

	cd::SkinID skinID = m_skinIDGenerator.AllocateID();
	uint32_t influenceBoneCount = pSkin->GetClusterCount();
	uint32_t meshVertexCount = sourceMesh.GetVertexPositionCount();

	cd::Skin skin;
	skin.SetID(skinID);
	skin.SetMeshID(sourceMesh.GetID());
	skin.SetName(pSkin->GetName());
	skin.SetVertexBoneNameArrayCount(meshVertexCount);
	skin.SetVertexBoneWeightArrayCount(meshVertexCount);

	auto& influenceBoneNames = skin.GetInfluenceBoneNames();
	for (int32_t skinClusterIndex = 0; skinClusterIndex < pSkin->GetClusterCount(); ++skinClusterIndex)
	{
		const fbxsdk::FbxCluster* pSkinCluster = pSkin->GetCluster(skinClusterIndex);
		const fbxsdk::FbxNode* pLinkBone = pSkinCluster->GetLink();
		if (!pLinkBone)
		{
			continue;
		}

		const char* pBoneName = pLinkBone->GetName();
		cd::Bone* pBone = pSceneDatabase->GetBoneByName(pBoneName);
		if (!pBone)
		{
			continue;
		}
		
		if (!skin.GetSkeletonID().IsValid())
		{
			skin.SetSkeletonID(pBone->GetSkeletonID());
		}
		else
		{
			assert(skin.GetSkeletonID() == pBone->GetSkeletonID());
		}

		auto& influenceBoneNames = skin.GetInfluenceBoneNames();
		// Two skin clusters created by same bone?
		assert(std::find(influenceBoneNames.begin(), influenceBoneNames.end(), pBoneName) == influenceBoneNames.end());
		skin.AddInfluenceBoneName(pBoneName);

		const int32_t controlPointIndicesCount = pSkinCluster->GetControlPointIndicesCount();
		int* pControlPointIndices = pSkinCluster->GetControlPointIndices();
		double* pBoneWeights = pSkinCluster->GetControlPointWeights();
		for (int32_t controlPointIndex = 0; controlPointIndex < controlPointIndicesCount; ++controlPointIndex)
		{
			uint32_t vertexIndex = pControlPointIndices[controlPointIndex];
			assert(vertexIndex < meshVertexCount);
			
			auto boneWeight = static_cast<float>(pBoneWeights[controlPointIndex]);
			skin.GetVertexBoneNameArray(vertexIndex).push_back(pBoneName);
			skin.GetVertexBoneWeightArray(vertexIndex).push_back(boneWeight);
		}
	}

	uint32_t maxVertexInfluenceCount = 0U;
	for (uint32_t vertexIndex = 0U; vertexIndex < meshVertexCount; ++vertexIndex)
	{
		uint32_t arraySize = static_cast<uint32_t>(skin.GetVertexBoneNameArray(vertexIndex).size());
		if (arraySize > maxVertexInfluenceCount)
		{
			maxVertexInfluenceCount = arraySize;
		}
	}
	skin.SetMaxVertexInfluenceCount(maxVertexInfluenceCount);

	if (skin.GetInfluenceBoneNameCount() > 0U)
	{
		pSceneDatabase->AddSkin(cd::MoveTemp(skin));
	}
	
	return cd::SkinID::InvalidID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// BlendShape
//////////////////////////////////////////////////////////////////////////////////////////////////////
cd::BlendShapeID FbxProducerImpl::ImportBlendShape(const fbxsdk::FbxBlendShape* pBlendShape, const cd::Mesh& sourceMesh, cd::SceneDatabase* pSceneDatabase)
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

	if (blendShape.GetMorphIDCount() > 0U)
	{
		pSceneDatabase->AddBlendShape(cd::MoveTemp(blendShape));
		return blendShapeID;
	}

	return cd::BlendShapeID::InvalidID;
}

void FbxProducerImpl::AssociateMeshWithBlendShape(fbxsdk::FbxMesh* pMesh, cd::MeshID meshID, cd::SceneDatabase* pSceneDatabase)
{
	if (!IsOptionEnabled(FbxProducerOptions::ImportBlendShape))
	{
		return;
	}

	auto& mesh = pSceneDatabase->GetMesh(meshID.Data());
	int32_t blendShapeCount = pMesh->GetDeformerCount(fbxsdk::FbxDeformer::eBlendShape);
	for (int32_t blendShapeIndex = 0; blendShapeIndex < blendShapeCount; ++blendShapeIndex)
	{
		const auto* pBlendShape = static_cast<fbxsdk::FbxBlendShape*>(pMesh->GetDeformer(blendShapeIndex, fbxsdk::FbxDeformer::eBlendShape));
		auto blendShapeID = ImportBlendShape(pBlendShape, mesh, pSceneDatabase);
		if (blendShapeID.IsValid())
		{
			mesh.AddBlendShapeID(blendShapeID);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Animation
//////////////////////////////////////////////////////////////////////////////////////////////////////
void FbxProducerImpl::ImportAnimation(fbxsdk::FbxScene* scene, cd::SceneDatabase* pSceneDatabase)
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
			cd::BoneID parentBoneID = cd::BoneID::Invalid();
			fbxsdk::FbxNodeAttribute* pParentNodeAttribute = pParent->GetNodeAttribute();
			if (pParentNodeAttribute && fbxsdk::FbxNodeAttribute::eSkeleton == pParentNodeAttribute->GetAttributeType())
			{
				parentBoneID = pSceneDatabase->GetBoneByName(pParent->GetName())->GetID();
			}

			for (uint32_t trackIndex = 0; trackIndex < trackCount; ++trackIndex)
			{
				//Build local matrix;
				cd::Matrix4x4 localMatrix;
				if (parentBoneID.IsValid())
				{
					auto& boneWorldInverseMatrices = worldInverseMatrices[parentBoneID.Data()];
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
