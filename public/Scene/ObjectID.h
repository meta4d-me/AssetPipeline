#pragma once

#include "Base/Export.h"
#include "Scene/ObjectType.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace cd
{

template<typename T, ObjectType>
class ObjectID final
{
public:
	using ValueType = T;
	static constexpr T InvalidID = std::numeric_limits<T>().max();
	static constexpr T MinID = static_cast<T>(0);
	static constexpr T MaxID = std::numeric_limits<T>().max() - 1;

	static constexpr ObjectID Invalid() { return ObjectID(InvalidID); }
	static constexpr ObjectID Min() { return ObjectID(MinID); }
	static constexpr ObjectID Max() { return ObjectID(MaxID); }

public:
	ObjectID() = default;
	ObjectID(T id) : m_id(id) {}
	ObjectID(const ObjectID&) = default;
	ObjectID& operator=(const ObjectID&) = default;
	ObjectID(ObjectID&&) = default;
	ObjectID& operator=(ObjectID&&) = default;
	~ObjectID() = default;
	T Data() const { return m_id; }
	void Set(T id) { m_id = id; }

	ObjectID& operator=(T id) { m_id = id; return *this; } 
	bool IsValid() const { return m_id != InvalidID; }
	bool operator==(const ObjectID& other) const { return m_id == other.m_id; }
	bool operator==(T id) const { return m_id == id; }
	bool operator!=(const ObjectID& other) const { return m_id != other.m_id; }
	bool operator!=(T id) const { return m_id != id; }
	bool operator<(const ObjectID& other) const { return m_id < other.m_id; }
	bool operator<(T id) const { return m_id < id; }
	bool operator<=(const ObjectID& other) const { return m_id <= other.m_id; }
	bool operator<=(T id) const { return m_id <= id; }
	bool operator>(const ObjectID& other) const { return m_id > other.m_id; }
	bool operator>(T id) const { return m_id > id; }
	bool operator>=(const ObjectID& other) const { return m_id >= other.m_id; }
	bool operator>=(T id) const { return m_id >= id; }

private:
	T m_id = InvalidID;
};

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

static_assert(sizeof(VertexID) == sizeof(uint32_t));

using VertexIDArray = std::vector<VertexID>;
using PolygonIDArray = std::vector<PolygonID>;

}