#pragma once

#include "Container/DynamicArray.hpp"
#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"

#include <cassert>
#include <vector>

namespace cd::pm
{

class Face
{
public:
	Face() = delete;
	explicit Face(cd::FaceID id) : m_id(id) { }
	Face(const Face&) = default;
	Face& operator=(const Face&) = default;
	Face(Face&&) = default;
	Face& operator=(Face&&) = default;
	~Face() = default;

	void SetID(cd::FaceID id) { m_id = id; }
	cd::FaceID GetID() const { return m_id; }

	void SetNormal(cd::Direction normal) { m_normal = normal; }
	cd::Direction& GetNormal() { return m_normal; }
	const cd::Direction& GetNormal() const { return m_normal; }

	void SetVertexID(uint32_t index, cd::VertexID vertexID) { m_vertexIDs[index] = vertexID; }
	cd::VertexID GetVertexID(uint32_t vertexIndex) const { return m_vertexIDs[vertexIndex]; }
	cd::DynamicArray<cd::VertexID>& GetVertexIDs() { return m_vertexIDs; }
	const cd::DynamicArray<cd::VertexID>& GetVertexIDs() const { return m_vertexIDs; }

private:
	// data
	cd::FaceID m_id;
	cd::Direction m_normal;
	
	// relationship
	cd::DynamicArray<VertexID> m_vertexIDs;
};

}