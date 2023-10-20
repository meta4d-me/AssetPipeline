#pragma once

#include "Framework/IProducer.h"

namespace fbxsdk
{

class FbxManager;
class FbxNode;
class FbxGeometryConverter;

}

namespace cdtools
{

class FbxProducerImpl;

class TOOL_API FbxProducer final : public IProducer
{
public:
	FbxProducer() = delete;
	explicit FbxProducer(const char* pFilePath);
	FbxProducer(const FbxProducer&) = delete;
	FbxProducer& operator=(const FbxProducer&) = delete;
	FbxProducer(FbxProducer&&) = delete;
	FbxProducer& operator=(FbxProducer&&) = delete;
	virtual ~FbxProducer();

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override;

	// Want to import materials.
	void SetWantImportMaterial(bool flag);
	bool WantImportMaterial() const;

	// Want to import textures.
	void SetWantImportTexture(bool flag);
	bool WantImportTexture() const;

	// Want to import static mesh or skin mesh.
	void SetWantImportSkinMesh(bool flag);
	bool WantImportSkinMesh() const;

	// Want to import animation stacks or not.
	void SetWantImportAnimation(bool flag);
	bool WantImportAnimation() const;

	void SetWantTriangulate(bool flag);
	bool IsTriangulateActive() const;

private:
	FbxProducerImpl* m_pFbxProducerImpl;
};

}