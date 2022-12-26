#pragma once

#include "Base/Export.h"
#include "ObjectType.h"

#include <cstdint>
#include <limits>

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

public:
	ObjectID() = default;
	explicit ObjectID(T id) : m_id(id) {}
	ObjectID(const ObjectID&) = default;
	ObjectID& operator=(const ObjectID&) = default;
	ObjectID(ObjectID&&) = default;
	ObjectID& operator=(ObjectID&&) = default;
	~ObjectID() = default;
	const T& Data() const { return m_id; }

	ObjectID& operator=(T id) { m_id = id; return *this; } 

	bool operator==(const ObjectID& other) { return m_id == other.m_id; }
	bool operator==(T id) { return m_id == id; }
	bool operator!=(const ObjectID& other) { return m_id != other.m_id; }
	bool operator!=(T id) { return m_id != id; }

private:
	T m_id = InvalidID;
};

using VertexID = ObjectID<uint32_t, ObjectType::Vertex>;
using EdgeID = ObjectID<uint32_t, ObjectType::Edge>;
using PolygonID = ObjectID<uint32_t, ObjectType::Polygon>;
using MeshID = ObjectID<uint32_t, ObjectType::Mesh>;
using MaterialID = ObjectID<uint32_t, ObjectType::Material>;
using TextureID = ObjectID<uint32_t, ObjectType::Texture>;
using LightID = ObjectID<uint32_t, ObjectType::Light>;

}