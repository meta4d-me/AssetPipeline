#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <array>
#include <string>
#include <vector>

namespace cd
{

class MorphImpl final
{
public:
	MorphImpl() = delete;

	template<bool SwapBytesOrder>
	explicit MorphImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit MorphImpl(uint32_t vertexCount);
	explicit MorphImpl(MorphID id, MeshID sourceMeshID, std::string name, uint32_t vertexCount);

	MorphImpl(const MorphImpl&) = default;
	MorphImpl& operator=(const MorphImpl&) = default;
	MorphImpl(MorphImpl&&) = default;
	MorphImpl& operator=(MorphImpl&&) = default;
	~MorphImpl() = default;

	void Init(uint32_t vertexCount);
	void Init(MorphID id, std::string name, uint32_t vertexCount);
	MorphID GetID() const { return m_id; }
	const std::string& GetName() const { return m_name; }

	MeshID GetSourceMeshID() const { return m_sourceMeshID; }

	void SetWeight(float weight) { m_weight = weight; }
	float& GetWeight() { return m_weight; } 
	float GetWeight() const { return m_weight; }

	uint32_t GetVertexCount() const { return m_vertexCount; }

	void SetVertexSourceID(uint32_t vertexIndex, uint32_t sourceID);
	VertexID GetVertexSourceID(uint32_t vertexIndex) const { return m_vertexSourceIDs[vertexIndex]; }
	std::vector<VertexID>& GetVertexSourceIDs() { return m_vertexSourceIDs; }
	const std::vector<VertexID>& GetVertexSourceIDs() const { return m_vertexSourceIDs; }

	void SetVertexPosition(uint32_t vertexIndex, const Point& position);
	Point& GetVertexPosition(uint32_t vertexIndex) { return m_vertexPositions[vertexIndex]; }
	const Point& GetVertexPosition(uint32_t vertexIndex) const { return m_vertexPositions[vertexIndex]; }
	std::vector<Point>& GetVertexPositions() { return m_vertexPositions; }
	const std::vector<Point>& GetVertexPositions() const { return m_vertexPositions; }

	void SetVertexNormal(uint32_t vertexIndex, const Direction& normal);
	Direction& GetVertexNormal(uint32_t vertexIndex) { return m_vertexNormals[vertexIndex]; }
	const Direction& GetVertexNormal(uint32_t vertexIndex) const { return m_vertexNormals[vertexIndex]; }
	std::vector<Direction>& GetVertexNormals() { return m_vertexNormals; }
	const std::vector<Direction>& GetVertexNormals() const { return m_vertexNormals; }

	void SetVertexTangent(uint32_t vertexIndex, const Direction& tangent);
	Direction& GetVertexTangent(uint32_t vertexIndex) { return m_vertexTangents[vertexIndex]; }
	const Direction& GetVertexTangent(uint32_t vertexIndex) const { return m_vertexTangents[vertexIndex]; }
	std::vector<Direction>& GetVertexTangents() { return m_vertexTangents; }
	const std::vector<Direction>& GetVertexTangents() const { return m_vertexTangents; }

	void SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent);
	Direction& GetVertexBiTangent(uint32_t vertexIndex) { return m_vertexBiTangents[vertexIndex]; }
	const Direction& GetVertexBiTangent(uint32_t vertexIndex) const { return m_vertexBiTangents[vertexIndex]; }
	std::vector<Direction>& GetVertexBiTangents() { return m_vertexBiTangents; }
	const std::vector<Direction>& GetVertexBiTangents() const { return m_vertexBiTangents; }

	template<bool SwapBytesOrder>
	MorphImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string name;
		uint32_t id;
		uint32_t vertexCount;
		inputArchive >> name >> id >> vertexCount;

		Init(MorphID(id), MoveTemp(name), vertexCount);
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MorphImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetVertexCount();
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());
		outputArchive.ExportBuffer(GetVertexNormals().data(), GetVertexNormals().size());
		outputArchive.ExportBuffer(GetVertexTangents().data(), GetVertexTangents().size());
		outputArchive.ExportBuffer(GetVertexBiTangents().data(), GetVertexBiTangents().size());

		return *this;
	}

private:
	std::string					m_name;
	MorphID						m_id;
	MeshID						m_sourceMeshID;
	float						m_weight;

	uint32_t					m_vertexCount;
	std::vector<VertexID>		m_vertexSourceIDs;

	// vertex geometry data
	std::vector<Point>			m_vertexPositions;
	std::vector<Direction>		m_vertexNormals;
	std::vector<Direction>		m_vertexTangents;
	std::vector<Direction>		m_vertexBiTangents;
};

}