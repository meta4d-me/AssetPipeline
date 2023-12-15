#pragma once

// Animation
struct AnimationTypeTraits
{
	// Simple
	using ID = cd::AnimationID;
	using Duration = float;
	using TicksPerSecond = float;

	// Vector
	using BoneTrackID = cd::TrackID;

	// String
	using Name = std::string;
};

// Bone
struct BoneTypeTraits
{
	// Simple
	using ID = cd::BoneID;
	using ParentID = BoneID;

	// Complex
	using Offset = cd::Matrix4x4;
	using Transform = cd::Transform;

	// String
	using Name = std::string;
};

// Camera
struct CameraTypeTraits
{
	// Simple
	using ID = cd::CameraID;
	using Aspect = float;
	using Fov = float;
	using NearPlane = float;
	using FarPlane = float;

	// Complex
	using Eye = cd::Point;
	using LookAt = cd::Direction;
	using Up = cd::Direction;

	// String
	using Name = std::string;
};

// Light
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

	// Complex
	using Color = cd::Vec3f;
	using Position = cd::Point;
	using Direction = cd::Direction;
	using Up = cd::Direction;

	// String
	using Name = std::string;
};

struct MaterialTypeTraits
{
	// Simple
	using ID = cd::MaterialID;
	using Type = cd::MaterialType;

	// String
	using Name = std::string;
};

// Mesh
struct MeshTypeTraits
{
	// Simple
	using ID = cd::MeshID;
	using MaterialID = cd::MaterialID;
	using VertexCount = uint32_t;
	using PolygonCount = uint32_t;

	// Complex
	using AABB = cd::AABB;
	using VertexFormat = cd::VertexFormat;

	// Vector
	using MorphID = cd::MorphID;
	using VertexPosition = cd::Point;
	using VertexNormal = cd::Direction;
	using VertexTangent = cd::Direction;
	using VertexBiTangent = cd::Direction;

	// String
	using Name = std::string;
};

// Morph
struct MorphTypeTraits
{
	// Simple
	using ID = cd::MorphID;
	using SourceMeshID = cd::MeshID;
	using Weight = float;
	using VertexCount = uint32_t;

	// Vector
	using VertexSourceID = cd::VertexID;
	using VertexPosition = cd::Point;
	using VertexNormal = cd::Direction;
	using VertexTangent = cd::Direction;
	using VertexBiTangent = cd::Direction;

	// String
	using Name = std::string;
};

// Node
struct NodeTypeTraits
{
	// Simple
	using ID = cd::NodeID;
	using ParentID = cd::NodeID;

	// Complex
	using Transform = cd::Transform;

	// Vector
	using ChildID = cd::NodeID;
	using MeshID = cd::MeshID;

	// String
	using Name = std::string;
};

// ParticleEmitter
struct ParticleEmitterTypeTraits
{
	// Simple
	using ID = cd::ParticleEmitterID;
	using Type = cd::ParticleEmitterType;

	// Complex
	using Position = cd::Vec3f;
	using Velocity = cd::Vec3f;
	using Accelerate = cd::Vec3f;
	using Color = cd::Vec4f;
	using FixedRotation = cd::Vec3f;
	using FixedScale = cd::Vec3f;

	// String
	using Name = std::string;
};

// Texture
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

	// Complex
	using UVOffset = cd::Vec2f;
	using UVScale = cd::Vec2f;
	using RawData = std::vector<std::byte>;

	// String
	using Name = std::string;
	using Path = std::string;
};

// Track
struct TrackTypeTraits
{
	// Simple
	using ID = cd::TrackID;

	// Complex
	using TranslationKey = cd::TranslationKey;
	using RotationKey = cd::RotationKey;
	using ScaleKey = cd::ScaleKey;

	// String
	using Name = std::string;
};