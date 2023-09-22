#pragma once

#include "ProgressiveMesh/ForwardDecls.h"

namespace cd::pm
{

class CORE_API Face
{
public:
	Face() = delete;
	explicit Face(FaceID id) : m_id(id) { }
	Face(const Face&) = default;
	Face& operator=(const Face&) = default;
	Face(Face&&) = default;
	Face& operator=(Face&&) = default;
	~Face() = default;

	void SetID(FaceID id) { m_id = id; }
	FaceID GetID() const { return m_id; }

	void SetNormal(Direction normal) { m_normal = normal; }
	Direction& GetNormal() { return m_normal; }
	const Direction& GetNormal() const { return m_normal; }

	void SetVertexID(uint32_t vertexIndex, VertexID vertexID) { m_vertexIDs[vertexIndex] = vertexID; }
	void SetVertexIDs(std::vector<VertexID> vertexIDs) { m_vertexIDs = cd::MoveTemp(vertexIDs); }
	VertexID GetVertexID(uint32_t vertexIndex) const { return m_vertexIDs[vertexIndex]; }
	std::vector<VertexID>& GetVertexIDs() { return m_vertexIDs; }
	const std::vector<VertexID>& GetVertexIDs() const { return m_vertexIDs; }

private:
	// data
	FaceID m_id;
	Direction m_normal;
	
	// relationship
	std::vector<VertexID> m_vertexIDs;
};

}