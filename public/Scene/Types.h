#pragma once

#include "Math/UnitSystem.hpp"
#include "Math/Transform.hpp"
#include "Scene/APIMacros.h"
#include "Scene/KeyFrame.hpp"
#include "Scene/LightType.h"
#include "Scene/MaterialTextureType.h"
#include "Scene/ObjectID.h"
#include "Scene/ParticleEmitterType.h"
#include "Scene/TextureFormat.h"
#include "Scene/VertexFormat.h"

namespace cd
{

// ID
using VertexID = ObjectID<uint32_t, ObjectType::Vertex>;
using EdgeID = ObjectID<uint32_t, ObjectType::Edge>;
using HalfEdgeID = ObjectID<uint32_t, ObjectType::HalfEdge>;
using FaceID = ObjectID<uint32_t, ObjectType::Face>;
using PolygonID = ObjectID<uint32_t, ObjectType::Polygon>;
using MeshID = ObjectID<uint32_t, ObjectType::Mesh>;
using MaterialID = ObjectID<uint32_t, ObjectType::Material>;
using TextureID = ObjectID<uint32_t, ObjectType::Texture>;
using LightID = ObjectID<uint32_t, ObjectType::Light>;
using NodeID = ObjectID<uint32_t, ObjectType::Node>;
using CameraID = ObjectID<uint32_t, ObjectType::Camera>;
using BoneID = ObjectID<uint32_t, ObjectType::Bone>;
using AnimationID = ObjectID<uint32_t, ObjectType::Animation>;
using TrackID = ObjectID<uint32_t, ObjectType::Track>;
using MorphID = ObjectID<uint32_t, ObjectType::Morph>;
using ParticleEmitterID = ObjectID<uint32_t, ObjectType::ParticleEmitter>;
using SkeletonID = ObjectID<uint32_t, ObjectType::Skeleton>;
using SkinID = ObjectID<uint32_t, ObjectType::Skin>;
static_assert(sizeof(VertexID) == sizeof(uint32_t));

// ID Array
using VertexIDArray = std::vector<VertexID>;
using PolygonIDArray = std::vector<PolygonID>;

// Geometry
using EdgePair = std::pair<VertexID, VertexID>;
using Triangle = TVector<VertexID, 3>;
using Quad = TVector<VertexID, 4>;
using Polygon = std::vector<VertexID>;

// Vector
using Point = cd::Vec3f;
using Position = Point;
using Direction = cd::Vec3f;
using Color = cd::Vec4f;
using UV = cd::Vec2f;
using RGB = cd::Vec3f;

}

// Scene API
#include "Scene/APITypeTraits.inl"