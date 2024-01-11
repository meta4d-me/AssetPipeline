#pragma once

namespace cd
{

class Animation;
class BlendShape;
class Bone;
class Camera;
class Light;
class Material;
class Mesh;
class Morph;
class Node;
class ParticleEmitter;
class Skeleton;
class Skin;
class Texture;
class Track;

struct SceneDatabaseTypeTraits
{
	// Simple
	using Unit = cd::Unit;

	// String
	using Name = std::string;

	// Complex
	using AABB = cd::AABB;
	using AxisSystem = cd::AxisSystem;

	// Vector
	using Animation = cd::Animation;
	using BlendShape = cd::BlendShape;
	using Bone = cd::Bone;
	using Camera = cd::Camera;
	using Light = cd::Light;
	using Material = cd::Material;
	using Mesh = cd::Mesh;
	using Morph = cd::Morph;
	using Node = cd::Node;
	using ParticleEmitter = cd::ParticleEmitter;
	using Skin = cd::Skin;
	using Skeleton = cd::Skeleton;
	using Texture = cd::Texture;
	using Track = cd::Track;
};

struct AnimationTypeTraits
{
	// Simple
	using ID = cd::AnimationID;
	using Duration = float;
	using TicksPerSecond = float;

	// String
	using Name = std::string;

	// Vector
	using BoneTrackID = cd::TrackID;
};

struct BlendShapeTypeTraits
{
	// Simple
	using ID = cd::BlendShapeID;
	using MeshID = cd::MeshID;

	// String
	using Name = std::string;

	// Vector
	using MorphID = cd::MorphID;
};

struct BoneTypeTraits
{
	// Simple
	using ID = cd::BoneID;
	using ParentID = cd::BoneID;
	using SkeletonID = cd::SkeletonID;
	using LimbLength = float;

	// String
	using Name = std::string;

	// Complex
	using Offset = cd::Matrix4x4;
	using Transform = cd::Transform;
	using LimbSize = cd::Vec3f;

	// Vector
	using ChildID = cd::BoneID;
};

struct CameraTypeTraits
{
	// Simple
	using ID = cd::CameraID;
	using Aspect = float;
	using Fov = float;
	using NearPlane = float;
	using FarPlane = float;

	// String
	using Name = std::string;

	// Complex
	using Eye = cd::Point;
	using LookAt = cd::Direction;
	using Up = cd::Direction;
};

struct LightTypeTraits
{
	// Simple
	using ID = cd::LightID;
	using Type = cd::LightType;
	using Intensity = float;
	using Range = float;
	using Radius = float;
	using Width = float;
	using Height = float;
	using AngleScale = float;
	using AngleOffset = float;

	// String
	using Name = std::string;

	// Complex
	using Color = cd::Vec3f;
	using Position = cd::Point;
	using Direction = cd::Direction;
	using Up = cd::Direction;
};

struct MaterialTypeTraits
{
	// Simple
	using ID = cd::MaterialID;
	using Type = cd::MaterialType;

	// String
	using Name = std::string;
};

struct MeshTypeTraits
{
	// Simple
	using ID = cd::MeshID;
	using VertexAttributeCount = uint32_t;

	// String
	using Name = std::string;

	// Complex
	using AABB = cd::AABB;
	using VertexFormat = cd::VertexFormat;

	// Vector
	using MaterialID = cd::MaterialID;
	using BlendShapeID = cd::BlendShapeID;
	using SkinID = cd::SkinID;
	using VertexPosition = cd::Point;
	using VertexInstanceID = uint32_t;
	using VertexNormal = cd::Direction;
	using VertexTangent = cd::Direction;
	using VertexBiTangent = cd::Direction;
	using PolygonGroup = cd::PolygonGroup;
};

struct MorphTypeTraits
{
	// Simple
	using ID = cd::MorphID;
	using BlendShapeID = cd::BlendShapeID;
	using Weight = float;

	// String
	using Name = std::string;

	// Vector
	using VertexSourceID = cd::VertexID;
	using VertexPosition = cd::Point;
};

struct NodeTypeTraits
{
	// Simple
	using ID = cd::NodeID;
	using ParentID = cd::NodeID;

	// String
	using Name = std::string;

	// Complex
	using Transform = cd::Transform;

	// Vector
	using ChildID = cd::NodeID;
	using MeshID = cd::MeshID;
};

struct ParticleEmitterTypeTraits
{
	// Simple
	using ID = cd::ParticleEmitterID;
	using Type = cd::ParticleEmitterType;
	using MeshID = cd::MeshID;

	// String
	using Name = std::string;

	// Complex
	using MaxCount = int;
	using Position = cd::Vec3f;
	using Velocity = cd::Vec3f;
	using Accelerate = cd::Vec3f;
	using Color = cd::Vec4f;
	using FixedRotation = cd::Vec3f;
	using FixedScale = cd::Vec3f;
};

struct SkeletonTypeTraits
{
	// Simple
	using ID = cd::SkeletonID;
	using RootBoneID = cd::BoneID;

	// String
	using Name = std::string;

	// Vector
	using BoneID = cd::BoneID;
};

struct SkinTypeTraits
{
	// Simple
	using ID = cd::SkinID;
	using MeshID = cd::MeshID;
	using SkeletonID = cd::SkeletonID;
	using MaxVertexInfluenceCount = uint32_t;

	// String
	using Name = std::string;

	// Vector
	using InfluenceBoneName = std::string;
	using VertexBoneNameArray = std::vector<std::string>;
	using VertexBoneWeightArray = std::vector<float>;
};

struct TextureTypeTraits
{
	// Simple
	using ID = cd::TextureID;
	using Type = cd::MaterialTextureType;
	using Format = cd::TextureFormat;
	using UMapMode = cd::TextureMapMode;
	using VMapMode = cd::TextureMapMode;
	using UseMipMap = bool;
	using Width = float;
	using Height = float;
	using Depth = float;

	// String
	using Name = std::string;
	using Path = std::string;

	// Complex
	using UVOffset = cd::Vec2f;
	using UVScale = cd::Vec2f;
	using RawData = std::vector<std::byte>;
};

struct TrackTypeTraits
{
	// Simple
	using ID = cd::TrackID;

	// String
	using Name = std::string;

	// Complex
	using TranslationKey = cd::TranslationKey;
	using RotationKey = cd::RotationKey;
	using ScaleKey = cd::ScaleKey;
};

}