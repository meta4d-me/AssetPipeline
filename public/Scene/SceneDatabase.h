#pragma once

#include "Base/Endian.h"
#include "Base/Export.h"
#include "Base/Template.h"
#include "Math/Box.hpp"
#include "Math/UnitSystem.hpp"
#include "Scene/Animation.h"
#include "Scene/Bone.h"
#include "Scene/Camera.h"
#include "Scene/Light.h"
#include "Scene/Material.h"
#include "Scene/Mesh.h"
#include "Scene/Morph.h"
#include "Scene/Node.h"
#include "Scene/ParticleEmitter.h"
#include "Scene/Skeleton.h"
#include "Scene/Texture.h"
#include "Scene/Track.h"

#include <vector>

namespace cd
{

class SceneDatabaseImpl;

class CORE_API SceneDatabase final
{
public:
	static const char* GetClassName() { return "SceneDatabase"; }

public:
	explicit SceneDatabase();
	SceneDatabase(const SceneDatabase&) = default;
	SceneDatabase& operator=(const SceneDatabase&) = default;
	SceneDatabase(SceneDatabase&&);
	SceneDatabase& operator=(SceneDatabase&&);
	~SceneDatabase();

	EXPORT_SIMPLE_TYPE_APIS(SceneDatabase, Unit);
	EXPORT_COMPLEX_TYPE_APIS(SceneDatabase, AABB);
	EXPORT_COMPLEX_TYPE_APIS(SceneDatabase, AxisSystem);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Animation);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Bone);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Camera);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Light);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Material);
	EXPORT_VECTOR_TYPE_APIS_WITH_PLURAL(SceneDatabase, Mesh, es);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Morph);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Node);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, ParticleEmitter);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Skeleton);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Texture);
	EXPORT_VECTOR_TYPE_APIS(SceneDatabase, Track);
	EXPORT_STRING_TYPE_APIS(SceneDatabase, Name);

	// Bone
	Bone* GetBoneByName(const char* pName);
	const Bone* GetBoneByName(const char* pName) const;

	// Node
	Node* GetNodeByName(const char* pName);
	const Node* GetNodeByName(const char* pName) const;

	// Track
	Track* GetTrackByName(const char* pName);
	const Track* GetTrackByName(const char* pName) const;

	// Operations
	void Dump() const;
	void Validate() const;
	void Merge(cd::SceneDatabase&& scene);
	void UpdateAABB();

	// Serialization
	SceneDatabase& operator<<(InputArchive& inputArchive);
	SceneDatabase& operator<<(InputArchiveSwapBytes& inputArchive);
	const SceneDatabase& operator>>(OutputArchive& outputArchive) const;
	const SceneDatabase& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	SceneDatabaseImpl* m_pSceneDatabaseImpl = nullptr;
};

}