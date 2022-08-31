#pragma once

#include <limits>
#include <inttypes.h>

namespace cdtools
{

enum class ObjectType
{
	Vertex,
	Edge,
	Polygon,
	Mesh,
	Material,
	Texture,
	Light,
};

template<typename Value, ObjectType>
class ObjectID final
{
public:
	static constexpr Value InvalidID = std::numeric_limits<Value>().max();

public:
	ObjectID() = default;
	explicit ObjectID(Value id) : m_id(id) {}
	ObjectID(const ObjectID&) = default;
	ObjectID& operator=(const ObjectID&) = default;
	ObjectID(ObjectID&&) = default;
	ObjectID& operator=(ObjectID&&) = default;
	~ObjectID() = default;
	const Value& Data() const { return m_id; }

	ObjectID& operator=(Value id) { m_id = id; return *this; } 

	bool operator==(const ObjectID& other) { return m_id == other.m_id; }
	bool operator==(Value id) { return m_id == id; }
	bool operator!=(const ObjectID& other) { return m_id != other.m_id; }
	bool operator!=(Value id) { return m_id != id; }

private:
	Value m_id = InvalidID;
};

using VertexID = ObjectID<uint32_t, ObjectType::Vertex>;
using EdgeID = ObjectID<uint32_t, ObjectType::Edge>;
using PolygonID = ObjectID<uint32_t, ObjectType::Polygon>;
using MeshID = ObjectID<uint32_t, ObjectType::Mesh>;
using MaterialID = ObjectID<uint32_t, ObjectType::Material>;
using TextureID = ObjectID<uint32_t, ObjectType::Texture>;
using LightID = ObjectID<uint32_t, ObjectType::Light>;

}